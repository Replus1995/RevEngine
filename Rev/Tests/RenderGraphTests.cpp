#include "Rev/Render/RenderGraph/RenderGraphBuilder.h"
#include "Rev/Render/RHI/RHITexture.h"
#include <iostream>
#include <stdexcept>

using namespace Rev;
namespace
{
struct FParams { FRGTextureHandle Input, Output; };
class FTestTexture final : public FRHITexture
{
public:
	explicit FTestTexture(const FRHITextureDesc& Desc) : FRHITexture(Desc) {}
	const void* GetNativeHandle() const override { return nullptr; }
};
void Check(bool Value, const char* Message) { if (!Value) throw std::runtime_error(Message); }
FRGTextureDesc Desc() { return FRGTextureDesc::Create2D(16, 16, PF_R8G8B8A8, Math::FLinearColor(), ETextureCreateFlags::ColorTarget | ETextureCreateFlags::ShaderResource); }

void TestLinearAndCulling()
{
	FRGTransientResourcePool Pool; FRGBuilder Graph(Pool);
	auto Texture = Graph.CreateTexture(Desc(), FRGName("LinearTexture"));
	const auto& Produce = Graph.AddPass<FParams>(FRGName("Produce"), ERGPassFlags::Raster,
		[&](FRGPassBuilder& B, FParams& P) { P.Output = B.UseColorAttachment(0, Texture, RTL_Clear); }, [](FRHICommandList&, const FParams&) {});
	const auto& Consume = Graph.AddPass<FParams>(FRGName("Consume"), ERGPassFlags::Raster,
		[&](FRGPassBuilder& B, FParams& P) { P.Input = B.ReadTexture(Produce.Output); P.Output = B.UseColorAttachment(0, Produce.Output, RTL_Load); }, [](FRHICommandList&, const FParams&) {});
	Graph.AddPass<FParams>(FRGName("DeadPass"), ERGPassFlags::None, [](FRGPassBuilder&, FParams&) {}, [](FRHICommandList&, const FParams&) {});
	FRHITextureRef Extracted; Graph.ExtractTexture(Consume.Output, &Extracted); Graph.Compile();
	const std::string Dot = Graph.ExportGraphviz();
	Check(Dot.find("Produce") != std::string::npos && Dot.find("Consume") != std::string::npos, "linear passes were culled");
	Check(Dot.find("DeadPass") == std::string::npos, "dead pass was not culled");
	Check(Graph.DumpResourceLifetimes().find("[0, 1]") != std::string::npos, "incorrect resource lifetime");
}

void TestFanOutAndNeverCull()
{
	FRGTransientResourcePool Pool; FRGBuilder Graph(Pool); auto T = Graph.CreateTexture(Desc(), FRGName("FanOut"));
	const auto& P = Graph.AddPass<FParams>(FRGName("Root"), ERGPassFlags::Raster, [&](FRGPassBuilder& B, FParams& X) { X.Output = B.UseColorAttachment(0, T, RTL_Clear); }, [](FRHICommandList&, const FParams&) {});
	for (int I = 0; I != 2; ++I) Graph.AddPass<FParams>(FRGName(I ? "FanB" : "FanA"), ERGPassFlags::NeverCull, [&](FRGPassBuilder& B, FParams& X) { X.Input = B.ReadTexture(P.Output); }, [](FRHICommandList&, const FParams&) {});
	Graph.Compile(); const std::string Dot = Graph.ExportGraphviz();
	Check(Dot.find("FanA") != std::string::npos && Dot.find("FanB") != std::string::npos, "fan-out/NeverCull failed");
}

void TestInvalidUses()
{
	FRGTransientResourcePool Pool;
	{
		FRGBuilder Graph(Pool); auto T = Graph.CreateTexture(Desc(), FRGName("Unproduced")); bool Threw = false;
		try { Graph.AddPass<FParams>(FRGName("BadRead"), ERGPassFlags::None, [&](FRGPassBuilder& B, FParams& P) { P.Input = B.ReadTexture(T); }, [](FRHICommandList&, const FParams&) {}); } catch (const std::logic_error&) { Threw = true; }
		Check(Threw, "read-before-produce was accepted");
	}
	{
		FRGBuilder Graph(Pool); auto T = Graph.CreateTexture(Desc(), FRGName("Stale"));
		Graph.AddPass<FParams>(FRGName("FirstWrite"), ERGPassFlags::None, [&](FRGPassBuilder& B, FParams& P) { P.Output = B.WriteTexture(T, ERHIAccess::CopyDst); }, [](FRHICommandList&, const FParams&) {});
		bool Threw = false; try { Graph.AddPass<FParams>(FRGName("StaleWrite"), ERGPassFlags::None, [&](FRGPassBuilder& B, FParams& P) { P.Output = B.WriteTexture(T, ERHIAccess::CopyDst); }, [](FRHICommandList&, const FParams&) {}); } catch (const std::logic_error&) { Threw = true; }
		Check(Threw, "stale write handle was accepted");
	}
}

void TestPresentAndExtract()
{
	FRGTransientResourcePool Pool; FRGBuilder Graph(Pool);
	auto External = CreateRef<FTestTexture>(FRHITextureDesc::Create2D(16, 16, PF_R8G8B8A8));
	auto Back = Graph.RegisterExternalTexture(External, ERHIAccess::Present, ERHIAccess::Present, FRGName("Back"));
	const auto& P = Graph.AddPass<FParams>(FRGName("PresentWriter"), ERGPassFlags::Raster, [&](FRGPassBuilder& B, FParams& X) { X.Output = B.UseColorAttachment(0, Back, RTL_DontCare); }, [](FRHICommandList&, const FParams&) {});
	FRHITextureRef Out; Graph.Present(P.Output); Graph.ExtractTexture(P.Output, &Out); Graph.Compile();
	Check(Graph.ExportGraphviz().find("PresentWriter") != std::string::npos, "Present/Extract root failed");
}

void TestReversedZ()
{
	constexpr float Near = 0.1f, Far = 1000.0f;
	const Math::FMatrix4 Perspective = Math::FMatrix4::Perspective(Math::Radians(60.0f), 16.0f / 9.0f, Near, Far);
	auto DepthOf = [&](float ViewZ) { const float ClipZ = Perspective[2][2] * ViewZ + Perspective[3][2]; const float ClipW = Perspective[2][3] * ViewZ + Perspective[3][3]; return ClipZ / ClipW; };
	Check(std::abs(DepthOf(-Near) - 1.0f) < 0.0001f, "reversed-Z near plane is not 1");
	Check(std::abs(DepthOf(-Far)) < 0.0001f, "reversed-Z far plane is not 0");
	const Math::FMatrix4 Ortho = Math::FMatrix4::Othographic(-1, 1, -1, 1, Near, Far);
	auto OrthoDepth = [&](float ViewZ) { return Ortho[2][2] * ViewZ + Ortho[3][2]; };
	Check(std::abs(OrthoDepth(-Near) - 1.0f) < 0.0001f && std::abs(OrthoDepth(-Far)) < 0.0001f, "orthographic reversed-Z mapping failed");
}
}

int main()
{
	try { TestLinearAndCulling(); TestFanOutAndNeverCull(); TestInvalidUses(); TestPresentAndExtract(); TestReversedZ(); }
	catch (const std::exception& E) { std::cerr << E.what() << '\n'; return 1; }
	std::cout << "RenderGraph CPU tests passed\n"; return 0;
}
