#pragma once
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"
#include "Rev/Render/RenderGraph/RenderGraphResources.h"
#include "Rev/Render/RenderGraph/RenderGraphPass.h"
#include <functional>
#include <memory>

namespace Rev
{
class FRHICommandList;
class REV_API FRGBuilder
{
public:
	explicit FRGBuilder(FRGTransientResourcePool& InPool, FRGName InName = FRGName("RenderGraph"));
	FRGBuilder(const FRGBuilder&) = delete;
	FRGBuilder& operator=(const FRGBuilder&) = delete;
	~FRGBuilder();
	FRGTextureHandle CreateTexture(const FRGTextureDesc&, FRGName);
	FRGTextureHandle RegisterExternalTexture(FRHITextureRef, ERHIAccess InitialAccess, ERHIAccess FinalAccess, FRGName);
	void Present(FRGTextureHandle);
	void ExtractTexture(FRGTextureHandle, FRHITextureRef* OutTexture);
	template<class TParameters, class TSetupLambda, class TExecuteLambda>
	const TParameters& AddPass(FRGName, ERGPassFlags, ERGPassPhase, TSetupLambda&&, TExecuteLambda&&);
	void Compile();
	void Execute(FRHICommandList&);
	FRHITexture* GetTexture(FRGTextureHandle) const;
	std::string ExportGraphviz() const;
	std::string DumpResourceLifetimes() const;
private:
	struct FAccess
	{
		FRGTextureHandle Handle;
		ERHIAccess Access = ERHIAccess::Unknown;
		FRHITextureSubresourceRange Range;
		bool bWrite = false;
		int32 AttachmentSlot = -1;
		ERenderTargetLoadAction Load = RTL_DontCare;
		ERenderTargetStoreAction Store = RTS_Store;
		bool bReadOnly = false;
	};
	struct FPassBase
	{
		FRGName Name;
		ERGPassFlags Flags = ERGPassFlags::None;
		ERGPassPhase Phase = ERGPassPhase::BasePass;
		std::vector<FAccess> Accesses;
		std::vector<uint32> Dependencies;
		std::vector<FRHITextureBarrier> Barriers;
		bool bCulled = true;
		virtual ~FPassBase() = default;
		virtual void Run(FRHICommandList&) = 0;
	};
	template<class TParameters, class TExecuteLambda> struct TPass final : FPassBase
	{
		TParameters Parameters{};
		TExecuteLambda ExecuteLambda;
		explicit TPass(TExecuteLambda&& InExecute) : ExecuteLambda(std::move(InExecute)) {}
		void Run(FRHICommandList& Cmd) override { ExecuteLambda(Cmd, Parameters); }
	};
	struct FResource
	{
		FRGName Name;
		FRGTextureDesc Desc;
		FRHITextureRef Physical;
		ERHIAccess InitialAccess = ERHIAccess::Unknown;
		ERHIAccess FinalAccess = ERHIAccess::Unknown;
		std::vector<ERHIAccess> SubresourceAccesses;
		uint32 CurrentVersion = 0;
		std::vector<int32> Producers{ -1 };
		bool bExternal = false;
		int32 FirstUse = -1;
		int32 LastUse = -1;
	};
	struct FExtraction { FRGTextureHandle Handle; FRHITextureRef* Output = nullptr; };
	FRGTextureHandle ReadTexture(uint32, FRGTextureHandle, ERHIAccess, FRHITextureSubresourceRange);
	FRGTextureHandle WriteTexture(uint32, FRGTextureHandle, ERHIAccess, FRHITextureSubresourceRange, int32, ERenderTargetLoadAction, ERenderTargetStoreAction, bool);
	void ValidateHandle(FRGTextureHandle) const;
	FRHIRenderingInfo BuildRenderingInfo(const FPassBase&) const;
	FRHIRenderTargetLayout BuildRenderTargetLayout(const FPassBase&) const;
	FRGTransientResourcePool& Pool;
	FRGName Name;
	std::vector<FResource> Resources;
	std::vector<std::unique_ptr<FPassBase>> Passes;
	std::vector<uint32> ExecutionOrder;
	std::vector<FRGTextureHandle> PresentRoots;
	std::vector<FExtraction> Extractions;
	std::vector<FRHITextureBarrier> FinalBarriers;
	bool bCompiled = false;
	friend class FRGPassBuilder;
};

template<class TParameters, class TSetupLambda, class TExecuteLambda>
const TParameters& FRGBuilder::AddPass(FRGName InName, ERGPassFlags InFlags, ERGPassPhase InPhase, TSetupLambda&& InSetup, TExecuteLambda&& InExecute)
{
	using FExecute = std::decay_t<TExecuteLambda>;
	auto Pass = std::make_unique<TPass<TParameters, FExecute>>(std::forward<TExecuteLambda>(InExecute));
	Pass->Name = std::move(InName); Pass->Flags = InFlags; Pass->Phase = InPhase;
	const uint32 Index = uint32(Passes.size());
	auto* Result = Pass.get();
	Passes.push_back(std::move(Pass));
	FRGPassBuilder Builder(*this, Index);
	InSetup(Builder, Result->Parameters);
	return Result->Parameters;
}
}
