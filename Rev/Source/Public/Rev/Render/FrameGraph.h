#pragma once
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "fg/FrameGraph.hpp"
#include "fg/Blackboard.hpp"
#include <functional>

namespace Rev
{

using FFGHandle = int32;
const FFGHandle KInvalidHandle = -1;

class FFGTexture
{
public:
	using Desc = FRHITextureDesc;

    FFGTexture() = default;
    FFGTexture(const Desc& InDesc)
    {
        create(InDesc, nullptr);
    }
    FFGTexture(FFGTexture&&) noexcept = default;

    FORCEINLINE FRHITexture* GetTextureRHI() const
    {
        return TextureRHI.get();
    }

    void create(const Desc& InDesc, void*)
    {
        if (!TextureRHI)
            TextureRHI = GDynamicRHI->RHICreateTexture(InDesc);
    }
    void destroy(const Desc&, void*)
    {
        //TextureRHI.reset();
    }
    void preRead(const Desc&, uint32_t, void*) const {}
    void preWrite() const {}
    static const char* toString(const Desc&) { return "<I>texture</I>"; }

private:
    FRHITextureRef TextureRHI = nullptr;
};

class FFGTextureExternal
{
public:
    //using Desc = FRHITextureDesc;
    struct Desc {

    };
    FFGTextureExternal() : TextureRHI(nullptr) {}
    FFGTextureExternal(FRHITexture* InTextureRHI) : TextureRHI(InTextureRHI) {}
    FFGTextureExternal(FFGTextureExternal&&) noexcept = default;

    void create(const Desc& InDesc, void*){}
    void destroy(const Desc&, void*){}
    void preRead(const Desc&, uint32_t, void*) const {}
    void preWrite() const {}
    static const char* toString(const Desc&) { return "<I>texture external</I>"; }

private:
    FRHITexture* TextureRHI = nullptr;
};

struct FFGRenderTargets
{
    struct ColorEntry
    {
        FFGHandle Texture = KInvalidHandle;
        FFGHandle ResolveTexture = KInvalidHandle;
        ERenderTargetLoadAction LoadAction = RTL_DontCare;
        ERenderTargetStoreAction StoreAction = RTS_DontCare;
        uint8 MipIndex = 0;
        int16 ArraySlice = -1;

        bool IsValid() const
        {
            return Texture != KInvalidHandle;
        }
    };

    struct DepthStencilEntry
    {
        FFGHandle Texture = KInvalidHandle;
        FFGHandle ResolveTexture = KInvalidHandle;
        ERenderTargetLoadAction DepthLoadAction = RTL_DontCare;
        ERenderTargetStoreAction DepthStoreAction = RTS_DontCare;
        ERenderTargetLoadAction StencilLoadAction = RTL_DontCare;
        ERenderTargetStoreAction StencilStoreAction = RTS_DontCare;

        bool IsValid() const
        {
            return Texture != KInvalidHandle;
        }
    };

    uint32 GetNumColorTargets() const
    {
        uint32 Count = 0;
        for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
        {
            if(!ColorTargets[i].IsValid())
                break;
            Count++;
        }
        return Count;
    }

    std::array<ColorEntry, RTA_MaxColorAttachments> ColorTargets;
    DepthStencilEntry DepthStencilTarget;
};

struct FFGPassData
{
    FFGPassData() = default;
    virtual ~FFGPassData() = default;

    FFGPassData& operator=(const FFGPassData& Other)
    {
        RenderTargets = Other.RenderTargets;
        RenderPassRHI = Other.RenderPassRHI;
    }

    void SetColorTarget(uint8 InIndex, FFGHandle InTexture, FFGHandle InTextureResolve,
        ERenderTargetLoadAction InLoadAction,
        ERenderTargetStoreAction InStoreAction = RTS_Store,
        uint8 InMipIndex = 0,
        int16 InArraySlice = -1);

    void SetDepthStencilTarget(FFGHandle InTexture, FFGHandle InTextureResolve,
        ERenderTargetLoadAction InDepthLoadAction, 
        ERenderTargetLoadAction InStencilLoadAction,
        ERenderTargetStoreAction InDepthStoreAction = RTS_Store ,
        ERenderTargetStoreAction InStencilStoreAction = RTS_Store);

    void InitRHI(class FFGPassResources& Resources);

    FRHIRenderPass* GetRenderPassRHI() const { return RenderPassRHI.get(); }

protected:
    FFGRenderTargets RenderTargets;
    FRHIRenderPassRef RenderPassRHI = nullptr;
};

class FFGBuilder
{
public:
    FFGBuilder(FrameGraph::Builder& InBuilder)
        : Builder(InBuilder)
    {}
    ~FFGBuilder() {}

    template<typename FGResourceType>
    FFGHandle Create(const char* InName, const typename FGResourceType::Desc& InDesc) //create transient resource
    {
        return Builder.create<FGResourceType>(InName, InDesc);
    }

    FFGHandle Read(FFGHandle InHandle, uint32 InFlags = uint32(-1))
    {
        return Builder.read(InHandle, InFlags);
    }

    FFGHandle Write(FFGHandle InHandle, uint32 InFlags = uint32(-1))
    {
        return Builder.write(InHandle, InFlags);
    }

    void SetSideEffect()
    {
        Builder.setSideEffect();
    }

private:
    FrameGraph::Builder& Builder;
};

class FFGPassResources
{
public:
    FFGPassResources(FrameGraphPassResources& InResources)
        : Resources(InResources)
    {}
    ~FFGPassResources() {}

    template<typename FGResourceType>
    FGResourceType& Get(FFGHandle InHandle)
    {
        return Resources.get<FGResourceType>(InHandle);
    }

    template<typename FGResourceType>
    const typename FGResourceType::Desc& GetDesc(FFGHandle InHandle)
    {
        return Resources.getDescriptor<FGResourceType>(InHandle);
    }

private:
    FrameGraphPassResources& Resources;
};

class FSceneProxy;
struct FFGContextData
{
    FFGContextData(FRHICommandList& InRHICmdList, FSceneProxy* InSceneProxy)
        : RHICmdList(InRHICmdList)
        , SceneProxy(InSceneProxy)
    {
    }

    FRHICommandList& RHICmdList;
    FSceneProxy* SceneProxy = nullptr;
};

struct FFGViewData
{
    uint32 Width = 0;
    uint32 Height = 0;
};

class FFrameGraph
{
public:
    FFrameGraph()
    {
        Reset();
    }
    ~FFrameGraph() = default;

    void Reset()
    {
        Graph = CreateScope<FrameGraph>();
        Blackboard = CreateScope<FrameGraphBlackboard>();
    }

    void Compile()
    {
        Graph->compile();
    }

    void Execute(FFGContextData& ContextData)
    {
        Graph->execute(&ContextData);
    }

    template<typename PassDataType>
    using SetupFuncType = std::function<void(FFGBuilder&, PassDataType&)>;

    template<typename PassDataType>
    using ExecuteFuncType = std::function<void(const PassDataType&, FFGPassResources&, FFGContextData&)>;

    template<typename PassDataType>
    void AddPass(const char* InName, SetupFuncType<PassDataType>&& InSetup, ExecuteFuncType<PassDataType>&& InExecute, bool bAddToBlackboard = true)
    {
        static_assert(std::derived_from<PassDataType, FFGPassData>,  "Invalid frame graph pass data type.");

        auto SetupFuncImpl = [Setup = std::move(InSetup)]
            (FrameGraph::Builder& BuilderImpl, PassDataType& Data)
            {
                FFGBuilder Builder(BuilderImpl);
                Setup(Builder, Data);
            };

        auto ExecuteFuncImpl = [InName, Execute = std::move(InExecute)]
            (const PassDataType& Data, FrameGraphPassResources& ResourcesImpl, void* Context)
            {
                FFGPassResources Resources(ResourcesImpl);
                FFGContextData* ContextData = static_cast<FFGContextData*>(Context);
                FFGPassData* PassData = (FFGPassData*)(&Data);

                REV_CORE_ASSERT(PassData !=  nullptr);

                PassData->InitRHI(Resources);

                ContextData->RHICmdList.GetContext()->RHIBeginDebugLabel(InName, Math::FLinearColor(0.6f, 0.8f, 0.6f));
                ContextData->RHICmdList.GetContext()->RHIBeginRenderPass(PassData->GetRenderPassRHI());

                Execute(Data, Resources, *ContextData);

                ContextData->RHICmdList.GetContext()->RHIEndRenderPass();
                ContextData->RHICmdList.GetContext()->RHIEndDebugLabel();
            };

        auto& FGPass = Graph->addCallbackPass<PassDataType>(InName, std::move(SetupFuncImpl), std::move(ExecuteFuncImpl));
        if (bAddToBlackboard)
        {
            Blackboard->add<PassDataType>() = FGPass;
        }
    }

    template<typename FGResourceType>
    FFGHandle Import(const char* InName, const typename FGResourceType::Desc& InDesc, FGResourceType&& InResource)
    {
        return Graph->import<FGResourceType>(std::string_view(InName), InDesc, std::move(InResource));
    }

    template<typename FGResourceType>
    const typename FGResourceType::Desc& GetDesc(FFGHandle InHandle) const
    {
        return Graph->getDescriptor<FGResourceType>(InHandle);
    }

    bool IsValid(FFGHandle InHandle) const
    {
        return Graph->isValid(InHandle);
    }

    template <typename WriterType>
    std::ostream& DebugOutput(std::ostream& Stream, WriterType&& Writer) const
    {
        return Graph->debugOutput<WriterType>(std::forward<WriterType>(Writer));
    }

    //Blackboard apis

    template<typename PassDataType>
    const PassDataType& GetPassData() const
    {
        return Blackboard->get<PassDataType>();
    }

    template<typename PassDataType>
    PassDataType& GetPassData()
    {
        return Blackboard->get<PassDataType>();
    }

    template<typename PassDataType>
    const PassDataType* TryGetPassData() const
    {
        return Blackboard->try_get<PassDataType>();
    }
   
    template<typename PassDataType>
    PassDataType* TryGetPassData()
    {
        return Blackboard->try_get<PassDataType>();
    }

    template<typename PassDataType>
    bool HasPassData() const
    {
        return Blackboard->has<PassDataType>();
    }


private:
    Scope<FrameGraph> Graph;
    Scope<FrameGraphBlackboard> Blackboard;
};


}