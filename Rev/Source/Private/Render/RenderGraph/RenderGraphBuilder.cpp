#include "Rev/Render/RenderGraph/RenderGraphBuilder.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIContext.h"
#include <algorithm>
#include <deque>
#include <sstream>
#include <stdexcept>

namespace Rev
{
namespace RenderGraphBuilderPrivate
{
bool SameDesc(const FRHITextureDesc& A, const FRHITextureDesc& B)
{
	return A.Width == B.Width && A.Height == B.Height && A.Depth == B.Depth && A.ArraySize == B.ArraySize &&
		A.Dimension == B.Dimension && A.Format == B.Format && A.NumMips == B.NumMips && A.NumSamples == B.NumSamples && A.Flags == B.Flags &&
		A.ClearColor.RGBA == B.ClearColor.RGBA;
}
void AddUnique(std::vector<uint32>& Values, uint32 Value)
{
	if (std::find(Values.begin(), Values.end(), Value) == Values.end()) Values.push_back(Value);
}
}
using namespace RenderGraphBuilderPrivate;

FRHITextureRef FRGTransientResourcePool::AcquireTexture(const FRGTextureDesc& InDesc)
{
	for (FEntry& Entry : Textures)
		if (!Entry.bInUse && FrameSerial >= Entry.LastUsedFrame + FramesUntilReuse && SameDesc(Entry.Desc, InDesc)) { Entry.bInUse = true; return Entry.Texture; }
	FEntry& Entry = Textures.emplace_back();
	Entry.Desc = InDesc; Entry.Texture = GDynamicRHI->RHICreateTexture(InDesc); Entry.bInUse = true;
	return Entry.Texture;
}
void FRGTransientResourcePool::EndFrame() { for (FEntry& Entry : Textures) if (Entry.bInUse) { Entry.bInUse = false; Entry.LastUsedFrame = FrameSerial; } ++FrameSerial; }
void FRGTransientResourcePool::Reset() { Textures.clear(); FrameSerial = 0; }

FRGBuilder::FRGBuilder(FRGTransientResourcePool& InPool, FRGName InName) : Pool(InPool), Name(std::move(InName)) {}
FRGBuilder::~FRGBuilder() = default;

FRGTextureHandle FRGBuilder::CreateTexture(const FRGTextureDesc& InDesc, FRGName InName)
{
	FResource Resource; Resource.Name = std::move(InName); Resource.Desc = InDesc;
	Resources.push_back(std::move(Resource));
	return { uint32(Resources.size() - 1), 0 };
}

FRGTextureHandle FRGBuilder::RegisterExternalTexture(FRHITextureRef InTexture, ERHIAccess InitialAccess, ERHIAccess FinalAccess, FRGName InName)
{
	if (!InTexture) throw std::invalid_argument("RenderGraph external texture is null");
	FResource Resource; Resource.Name = std::move(InName); Resource.Desc = FRGTextureDesc(InTexture->GetDesc()); Resource.Physical = std::move(InTexture);
	Resource.InitialAccess = InitialAccess; Resource.FinalAccess = FinalAccess; Resource.bExternal = true;
	Resources.push_back(std::move(Resource));
	return { uint32(Resources.size() - 1), 0 };
}

void FRGBuilder::ValidateHandle(FRGTextureHandle Handle) const
{
	if (!Handle.IsValid() || Handle.ResourceIndex >= Resources.size()) throw std::out_of_range("Invalid RenderGraph texture handle");
	if (Handle.Version >= Resources[Handle.ResourceIndex].Producers.size()) throw std::out_of_range("Invalid RenderGraph texture version");
}

FRGTextureHandle FRGBuilder::ReadTexture(uint32 PassIndex, FRGTextureHandle Handle, ERHIAccess Access, FRHITextureSubresourceRange Range)
{
	ValidateHandle(Handle);
	const FResource& Resource = Resources[Handle.ResourceIndex];
	if (!Resource.bExternal && Resource.Producers[Handle.Version] < 0) throw std::logic_error("RenderGraph read before produce: " + Resource.Name.GetName());
	Passes[PassIndex]->Accesses.push_back({ Handle, Access, Range, false });
	return Handle;
}

FRGTextureHandle FRGBuilder::WriteTexture(uint32 PassIndex, FRGTextureHandle Handle, ERHIAccess Access, FRHITextureSubresourceRange Range,
	int32 AttachmentSlot, ERenderTargetLoadAction Load, ERenderTargetStoreAction Store, bool bReadOnly)
{
	ValidateHandle(Handle);
	FResource& Resource = Resources[Handle.ResourceIndex];
	if (bReadOnly)
	{
		ReadTexture(PassIndex, Handle, Access, Range);
		FAccess& Item = Passes[PassIndex]->Accesses.back(); Item.AttachmentSlot = AttachmentSlot; Item.Load = Load; Item.Store = Store; Item.bReadOnly = true;
		return Handle;
	}
	if (Handle.Version != Resource.CurrentVersion) throw std::logic_error("RenderGraph write through stale handle: " + Resource.Name.GetName());
	const int32 PreviousProducer = Resource.Producers[Handle.Version];
	if (Load == RTL_Load && PreviousProducer < 0 && !Resource.bExternal) throw std::logic_error("RenderGraph attachment LOAD before produce: " + Resource.Name.GetName());
	if (PreviousProducer >= 0 && PreviousProducer != int32(PassIndex)) AddUnique(Passes[PassIndex]->Dependencies, uint32(PreviousProducer));
	Handle.Version = ++Resource.CurrentVersion;
	Resource.Producers.push_back(int32(PassIndex));
	Passes[PassIndex]->Accesses.push_back({ Handle, Access, Range, true, AttachmentSlot, Load, Store, false });
	return Handle;
}

FRGTextureHandle FRGPassBuilder::ReadTexture(FRGTextureHandle H, ERHIAccess A, FRHITextureSubresourceRange R) { return Graph.ReadTexture(PassIndex, H, A, R); }
FRGTextureHandle FRGPassBuilder::WriteTexture(FRGTextureHandle H, ERHIAccess A, FRHITextureSubresourceRange R) { return Graph.WriteTexture(PassIndex, H, A, R, -1, RTL_DontCare, RTS_Store, false); }
FRGTextureHandle FRGPassBuilder::UseColorAttachment(uint32 Slot, FRGTextureHandle H, ERenderTargetLoadAction L, ERenderTargetStoreAction S)
{
	if (Slot >= REV_MAX_RENDER_TARGETS) throw std::out_of_range("RenderGraph color attachment slot");
	return Graph.WriteTexture(PassIndex, H, ERHIAccess::ColorAttachment, {}, int32(Slot), L, S, false);
}
FRGTextureHandle FRGPassBuilder::UseDepthStencil(FRGTextureHandle H, ERenderTargetLoadAction L, ERenderTargetStoreAction S, bool ReadOnly)
{
	return Graph.WriteTexture(PassIndex, H, ReadOnly ? ERHIAccess::DepthStencilRead : ERHIAccess::DepthStencilWrite, {}, -2, L, S, ReadOnly);
}

void FRGBuilder::Present(FRGTextureHandle Handle) { ValidateHandle(Handle); PresentRoots.push_back(Handle); Resources[Handle.ResourceIndex].FinalAccess = ERHIAccess::Present; }
void FRGBuilder::ExtractTexture(FRGTextureHandle Handle, FRHITextureRef* Out) { ValidateHandle(Handle); if (!Out) throw std::invalid_argument("Extract output is null"); Extractions.push_back({ Handle, Out }); }

void FRGBuilder::Compile()
{
	if (bCompiled) return;
	std::vector<int32> LastAccess(Resources.size(), -1);
	for (uint32 PassIndex = 0; PassIndex < Passes.size(); ++PassIndex)
	{
		FPassBase& Pass = *Passes[PassIndex];
		for (const FAccess& Access : Pass.Accesses)
		{
			const FResource& Resource = Resources[Access.Handle.ResourceIndex];
			const int32 Producer = Resource.Producers[Access.Handle.Version];
			if (!Access.bWrite && Producer >= 0 && Producer != int32(PassIndex)) AddUnique(Pass.Dependencies, uint32(Producer));
			if (LastAccess[Access.Handle.ResourceIndex] >= 0 && LastAccess[Access.Handle.ResourceIndex] != int32(PassIndex)) AddUnique(Pass.Dependencies, uint32(LastAccess[Access.Handle.ResourceIndex]));
			LastAccess[Access.Handle.ResourceIndex] = int32(PassIndex);
		}
	}
	std::vector<uint8> Live(Passes.size(), 0);
	std::function<void(uint32)> Mark = [&](uint32 Index) { if (Live[Index]) return; Live[Index] = 1; for (uint32 D : Passes[Index]->Dependencies) Mark(D); };
	for (const FRGTextureHandle Root : PresentRoots) { const int32 P = Resources[Root.ResourceIndex].Producers[Root.Version]; if (P >= 0) Mark(uint32(P)); }
	for (const FExtraction& Root : Extractions) { const int32 P = Resources[Root.Handle.ResourceIndex].Producers[Root.Handle.Version]; if (P >= 0) Mark(uint32(P)); }
	for (uint32 I = 0; I < Passes.size(); ++I) if (EnumHasAnyFlags(Passes[I]->Flags, ERGPassFlags::NeverCull)) Mark(I);
	std::vector<uint32> InDegree(Passes.size(), 0); std::vector<std::vector<uint32>> Consumers(Passes.size());
	for (uint32 I = 0; I < Passes.size(); ++I) if (Live[I]) for (uint32 D : Passes[I]->Dependencies) if (Live[D]) { ++InDegree[I]; Consumers[D].push_back(I); }
	std::deque<uint32> Ready; for (uint32 I = 0; I < Passes.size(); ++I) if (Live[I] && InDegree[I] == 0) Ready.push_back(I);
	while (!Ready.empty()) { uint32 I = Ready.front(); Ready.pop_front(); ExecutionOrder.push_back(I); for (uint32 C : Consumers[I]) if (--InDegree[C] == 0) Ready.push_back(C); }
	if (ExecutionOrder.size() != size_t(std::count(Live.begin(), Live.end(), uint8(1)))) throw std::logic_error("RenderGraph contains a cycle");
	for (FResource& Resource : Resources) { Resource.CurrentAccess = Resource.InitialAccess; if (!Resource.bExternal && GDynamicRHI) Resource.Physical = Pool.AcquireTexture(Resource.Desc); }
	for (uint32 OrderIndex = 0; OrderIndex < ExecutionOrder.size(); ++OrderIndex)
	{
		FPassBase& Pass = *Passes[ExecutionOrder[OrderIndex]]; Pass.bCulled = false;
		for (const FAccess& Access : Pass.Accesses)
		{
			FResource& Resource = Resources[Access.Handle.ResourceIndex];
			if (Resource.FirstUse < 0) Resource.FirstUse = int32(OrderIndex); Resource.LastUse = int32(OrderIndex);
			if (Resource.CurrentAccess != Access.Access) { Pass.Barriers.push_back({ Resource.Physical.get(), Resource.CurrentAccess, Access.Access, Access.Range }); Resource.CurrentAccess = Access.Access; }
		}
	}
	for (FResource& Resource : Resources) if (Resource.bExternal && Resource.FinalAccess != ERHIAccess::Unknown && Resource.CurrentAccess != Resource.FinalAccess)
		FinalBarriers.push_back({ Resource.Physical.get(), Resource.CurrentAccess, Resource.FinalAccess, {} });
	bCompiled = true;
}

FRHIRenderingInfo FRGBuilder::BuildRenderingInfo(const FPassBase& Pass) const
{
	FRHIRenderingInfo Info;
	for (const FAccess& A : Pass.Accesses) if (A.AttachmentSlot >= 0) { const FResource& R = Resources[A.Handle.ResourceIndex]; Info.Width = R.Desc.Width; Info.Height = R.Desc.Height; Info.NumColorAttachments = std::max<uint8>(Info.NumColorAttachments, uint8(A.AttachmentSlot + 1)); Info.ColorAttachments[A.AttachmentSlot] = { R.Physical.get(), nullptr, A.Range, A.Load, A.Store }; }
	for (const FAccess& A : Pass.Accesses) if (A.AttachmentSlot == -2) { const FResource& R = Resources[A.Handle.ResourceIndex]; Info.Width = R.Desc.Width; Info.Height = R.Desc.Height; Info.DepthAttachment = { R.Physical.get(), nullptr, A.Range, A.Load, A.Store }; Info.bDepthReadOnly = A.bReadOnly; if (FPixelFormatInfo::HasStencil(R.Desc.Format)) { Info.StencilAttachment = Info.DepthAttachment; Info.bStencilReadOnly = A.bReadOnly; } }
	return Info;
}
FRHIRenderTargetLayout FRGBuilder::BuildRenderTargetLayout(const FPassBase& Pass) const
{
	FRHIRenderTargetLayout Layout;
	for (const FAccess& A : Pass.Accesses) if (A.AttachmentSlot >= 0) { const FResource& R = Resources[A.Handle.ResourceIndex]; Layout.NumColorAttachments = std::max<uint8>(Layout.NumColorAttachments, uint8(A.AttachmentSlot + 1)); Layout.ColorFormats[A.AttachmentSlot] = R.Desc.Format; Layout.NumSamples = R.Desc.NumSamples; }
	for (const FAccess& A : Pass.Accesses) if (A.AttachmentSlot == -2) Layout.DepthStencilFormat = Resources[A.Handle.ResourceIndex].Desc.Format;
	return Layout;
}

void FRGBuilder::Execute(FRHICommandList& Cmd)
{
	if (!bCompiled) Compile();
	for (uint32 Index : ExecutionOrder)
	{
		FPassBase& Pass = *Passes[Index]; Cmd.Transition(Pass.Barriers);
		Cmd.BeginDebugLabel(Pass.Name.GetCharName(), Math::FLinearColor(0.55f, 0.8f, 0.65f));
		const bool bRaster = EnumHasAnyFlags(Pass.Flags, ERGPassFlags::Raster);
		if (bRaster) { Cmd.SetRenderTargetLayout(BuildRenderTargetLayout(Pass)); Cmd.BeginRendering(BuildRenderingInfo(Pass)); }
		Pass.Run(Cmd);
		if (bRaster) Cmd.EndRendering();
		Cmd.EndDebugLabel();
	}
	Cmd.Transition(FinalBarriers);
	for (const FExtraction& Extraction : Extractions) *Extraction.Output = Resources[Extraction.Handle.ResourceIndex].Physical;
	Pool.EndFrame();
}

FRHITexture* FRGBuilder::GetTexture(FRGTextureHandle H) const { ValidateHandle(H); return Resources[H.ResourceIndex].Physical.get(); }
std::string FRGBuilder::ExportGraphviz() const
{
	std::ostringstream Out; Out << "digraph RenderGraph {\n";
	for (uint32 I = 0; I < Passes.size(); ++I) if (!Passes[I]->bCulled) { Out << "  p" << I << " [label=\"" << Passes[I]->Name.GetName() << "\"];\n"; for (uint32 D : Passes[I]->Dependencies) if (!Passes[D]->bCulled) Out << "  p" << D << " -> p" << I << ";\n"; }
	return Out.str() + "}\n";
}
std::string FRGBuilder::DumpResourceLifetimes() const { std::ostringstream Out; for (const FResource& R : Resources) Out << R.Name.GetName() << ": [" << R.FirstUse << ", " << R.LastUse << "] v" << R.CurrentVersion << "\n"; return Out.str(); }
}
