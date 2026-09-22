#include "Rev/Render/RenderGraph/RenderGraphBuilder.h"
#include "Rev/Render/RenderOptions.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Asset/TextureStorage.h"
#include <iostream>
#include <stdexcept>

using namespace Rev;
namespace RenderGraphTestsPrivate
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
	const auto& Produce = Graph.AddPass<FParams>(FRGName("Produce"), ERGPassFlags::Raster, ERGPassPhase::BasePass,
		[&](FRGPassBuilder& B, FParams& P) { P.Output = B.UseColorAttachment(0, Texture, RTL_Clear); }, [](FRHICommandList&, const FParams&) {});
	const auto& Consume = Graph.AddPass<FParams>(FRGName("Consume"), ERGPassFlags::Raster, ERGPassPhase::BasePass,
		[&](FRGPassBuilder& B, FParams& P) { P.Input = B.ReadTexture(Produce.Output); P.Output = B.UseColorAttachment(0, Produce.Output, RTL_Load); }, [](FRHICommandList&, const FParams&) {});
	Graph.AddPass<FParams>(FRGName("DeadPass"), ERGPassFlags::None, ERGPassPhase::BasePass, [](FRGPassBuilder&, FParams&) {}, [](FRHICommandList&, const FParams&) {});
	FRHITextureRef Extracted; Graph.ExtractTexture(Consume.Output, &Extracted); Graph.Compile();
	const std::string Dot = Graph.ExportGraphviz();
	Check(Dot.find("Produce") != std::string::npos && Dot.find("Consume") != std::string::npos, "linear passes were culled");
	Check(Dot.find("DeadPass") == std::string::npos, "dead pass was not culled");
	Check(Graph.DumpResourceLifetimes().find("[0, 1]") != std::string::npos, "incorrect resource lifetime");
}
void TestFanOutAndNeverCull()
{
	FRGTransientResourcePool Pool; FRGBuilder Graph(Pool); auto T = Graph.CreateTexture(Desc(), FRGName("FanOut"));
	const auto& P = Graph.AddPass<FParams>(FRGName("Root"), ERGPassFlags::Raster, ERGPassPhase::BasePass, [&](FRGPassBuilder& B, FParams& X) { X.Output = B.UseColorAttachment(0, T, RTL_Clear); }, [](FRHICommandList&, const FParams&) {});
	for (int I = 0; I != 2; ++I) Graph.AddPass<FParams>(FRGName(I ? "FanB" : "FanA"), ERGPassFlags::NeverCull, ERGPassPhase::BasePass, [&](FRGPassBuilder& B, FParams& X) { X.Input = B.ReadTexture(P.Output); }, [](FRHICommandList&, const FParams&) {});
	Graph.Compile(); const std::string Dot = Graph.ExportGraphviz();
	Check(Dot.find("FanA") != std::string::npos && Dot.find("FanB") != std::string::npos, "fan-out/NeverCull failed");
}

void TestInvalidUses()
{
	FRGTransientResourcePool Pool;
	{
		FRGBuilder Graph(Pool); auto T = Graph.CreateTexture(Desc(), FRGName("Unproduced")); bool Threw = false;
		try { Graph.AddPass<FParams>(FRGName("BadRead"), ERGPassFlags::None, ERGPassPhase::BasePass, [&](FRGPassBuilder& B, FParams& P) { P.Input = B.ReadTexture(T); }, [](FRHICommandList&, const FParams&) {}); } catch (const std::logic_error&) { Threw = true; }
		Check(Threw, "read-before-produce was accepted");
	}
	{
		FRGBuilder Graph(Pool); auto T = Graph.CreateTexture(Desc(), FRGName("Stale"));
		Graph.AddPass<FParams>(FRGName("FirstWrite"), ERGPassFlags::None, ERGPassPhase::BasePass, [&](FRGPassBuilder& B, FParams& P) { P.Output = B.WriteTexture(T, ERHIAccess::CopyDst); }, [](FRHICommandList&, const FParams&) {});
		bool Threw = false; try { Graph.AddPass<FParams>(FRGName("StaleWrite"), ERGPassFlags::None, ERGPassPhase::BasePass, [&](FRGPassBuilder& B, FParams& P) { P.Output = B.WriteTexture(T, ERHIAccess::CopyDst); }, [](FRHICommandList&, const FParams&) {}); } catch (const std::logic_error&) { Threw = true; }
		Check(Threw, "stale write handle was accepted");
	}
}

void TestPresentAndExtract()
{
	FRGTransientResourcePool Pool; FRGBuilder Graph(Pool);
	auto External = CreateRef<FTestTexture>(FRHITextureDesc::Create2D(16, 16, PF_R8G8B8A8));
	auto Back = Graph.RegisterExternalTexture(External, ERHIAccess::Present, ERHIAccess::Present, FRGName("Back"));
	const auto& P = Graph.AddPass<FParams>(FRGName("PresentWriter"), ERGPassFlags::Raster, ERGPassPhase::PostProcess, [&](FRGPassBuilder& B, FParams& X) { X.Output = B.UseColorAttachment(0, Back, RTL_DontCare); }, [](FRHICommandList&, const FParams&) {});
	FRHITextureRef Out; Graph.Present(P.Output); Graph.ExtractTexture(P.Output, &Out); Graph.Compile();
	Check(Graph.ExportGraphviz().find("PresentWriter") != std::string::npos, "Present/Extract root failed");
}

void TestPassPhaseOrdering()
{
	FRGTransientResourcePool Pool; FRGBuilder Graph(Pool);
	auto BaseTexture = Graph.CreateTexture(Desc(), FRGName("BaseTexture"));
	auto ShadowTextureA = Graph.CreateTexture(Desc(), FRGName("ShadowTextureA"));
	auto ShadowTextureB = Graph.CreateTexture(Desc(), FRGName("ShadowTextureB"));
	const auto& Base = Graph.AddPass<FParams>(FRGName("Base"), ERGPassFlags::None, ERGPassPhase::BasePass,
		[&](FRGPassBuilder& B, FParams& P) { P.Output = B.WriteTexture(BaseTexture, ERHIAccess::CopyDst); }, [](FRHICommandList&, const FParams&) {});
	const auto& ShadowA = Graph.AddPass<FParams>(FRGName("ShadowA"), ERGPassFlags::None, ERGPassPhase::ShadowDepth,
		[&](FRGPassBuilder& B, FParams& P) { P.Output = B.WriteTexture(ShadowTextureA, ERHIAccess::CopyDst); }, [](FRHICommandList&, const FParams&) {});
	const auto& ShadowB = Graph.AddPass<FParams>(FRGName("ShadowB"), ERGPassFlags::None, ERGPassPhase::ShadowDepth,
		[&](FRGPassBuilder& B, FParams& P) { P.Output = B.WriteTexture(ShadowTextureB, ERHIAccess::CopyDst); }, [](FRHICommandList&, const FParams&) {});
	FRHITextureRef BaseOut, ShadowOutA, ShadowOutB;
	Graph.ExtractTexture(Base.Output, &BaseOut); Graph.ExtractTexture(ShadowA.Output, &ShadowOutA); Graph.ExtractTexture(ShadowB.Output, &ShadowOutB); Graph.Compile();
	const std::string Lifetimes = Graph.DumpResourceLifetimes();
	Check(Lifetimes.find("BaseTexture: [2, 2]") != std::string::npos, "BasePass executed before ShadowDepth");
	Check(Lifetimes.find("ShadowTextureA: [0, 0]") != std::string::npos, "ShadowDepth phase was not scheduled first");
	Check(Lifetimes.find("ShadowTextureB: [1, 1]") != std::string::npos, "same-phase registration order was not preserved");
}

void TestPassPhaseViolation()
{
	FRGTransientResourcePool Pool; FRGBuilder Graph(Pool);
	auto Texture = Graph.CreateTexture(Desc(), FRGName("PhaseViolation"));
	const auto& Base = Graph.AddPass<FParams>(FRGName("BaseProducer"), ERGPassFlags::None, ERGPassPhase::BasePass,
		[&](FRGPassBuilder& B, FParams& P) { P.Output = B.WriteTexture(Texture, ERHIAccess::CopyDst); }, [](FRHICommandList&, const FParams&) {});
	Graph.AddPass<FParams>(FRGName("ShadowConsumer"), ERGPassFlags::NeverCull, ERGPassPhase::ShadowDepth,
		[&](FRGPassBuilder& B, FParams& P) { P.Input = B.ReadTexture(Base.Output); }, [](FRHICommandList&, const FParams&) {});
	bool Threw = false;
	try { Graph.Compile(); } catch (const std::logic_error&) { Threw = true; }
	Check(Threw, "earlier phase was allowed to depend on a later phase");
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

void TestTextureDescriptors()
{
	Check(FRHITextureDesc::CalculateFullMipCount(16, 8) == 5, "2D mip count is incorrect");
	Check(FRHITextureDesc::CalculateFullMipCount(8, 8, 8) == 4, "3D mip count is incorrect");
	const FRHITextureDesc Array = FRHITextureDesc::Create2DArray(16, 16, 3, PF_R8G8B8A8);
	const FRHITextureDesc Cube = FRHITextureDesc::CreateCube(16, PF_R8G8B8A8);
	const FRHITextureDesc CubeArray = FRHITextureDesc::CreateCubeArray(16, 4, PF_R8G8B8A8);
	const FRHITextureDesc Volume = FRHITextureDesc::Create3D(16, 8, 4, PF_R8G8B8A8);
	Check(Array.GetPhysicalLayerCount() == 3 && Cube.GetPhysicalLayerCount() == 6 && CubeArray.GetPhysicalLayerCount() == 24 && Volume.GetPhysicalLayerCount() == 1, "physical layer counts are incorrect");
	Check(Volume.GetMipExtent(2) == Math::FVector3(4, 2, 1), "3D mip extent is incorrect");
	Check(Cube.Validate() && CubeArray.Validate() && Volume.Validate(), "valid texture descriptor was rejected");
	auto InvalidCube = FRHITextureDesc::CreateCube(16, PF_R8G8B8A8); InvalidCube.Height = 8;
	Check(!InvalidCube.Validate(), "invalid cube descriptor was accepted");
	FImageStorage Images; Images.Resize(3, 2); Images.At(2, 1).Allocate(1);
	Check(Images.NumMips() == 3 && Images.NumLayers() == 2, "image storage mip/layer layout is incorrect");
}

void TestDirectionalShadowOptions()
{
	Check(GRenderOptions.GetDirectionalShadowMode() == EDirectionalShadowMode::CascadedShadowMap, "directional shadow mode does not default to CSM");
	GRenderOptions.SetDirectionalShadowMode(EDirectionalShadowMode::ShadowMap);
	Check(GRenderOptions.GetDirectionalShadowMode() == EDirectionalShadowMode::ShadowMap, "directional shadow mode was not updated");
	FCSMSettings Settings;
	Settings.CascadeCount = 0;
	Settings.Resolution = 0;
	Settings.MaxDistance = 0.0f;
	Settings.SplitLambda = 2.0f;
	Settings.TransitionFraction = -1.0f;
	Settings.CasterExtrusion = -1.0f;
	GRenderOptions.SetCSMSettings(Settings);
	const FCSMSettings& Clamped = GRenderOptions.GetCSMSettings();
	Check(Clamped.CascadeCount == 1 && Clamped.Resolution == 1, "CSM integer settings were not clamped");
	Check(Clamped.MaxDistance == 0.01f && Clamped.SplitLambda == 1.0f && Clamped.TransitionFraction == 0.0f && Clamped.CasterExtrusion == 0.0f, "CSM float settings were not clamped");
}
}
using namespace RenderGraphTestsPrivate;

int main()
{
	try { TestLinearAndCulling(); TestFanOutAndNeverCull(); TestInvalidUses(); TestPresentAndExtract(); TestPassPhaseOrdering(); TestPassPhaseViolation(); TestReversedZ(); TestTextureDescriptors(); TestDirectionalShadowOptions(); }
	catch (const std::exception& E) { std::cerr << E.what() << '\n'; return 1; }
	std::cout << "RenderGraph CPU tests passed\n"; return 0;
}
