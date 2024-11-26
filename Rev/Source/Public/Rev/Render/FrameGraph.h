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
const FFGHandle KFGInvalidHandle = -1;

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
        TextureRHI.reset();
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
    FFGTextureExternal() = default;
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
        FFGHandle Texture = KFGInvalidHandle;
        FFGHandle ResolveTexture = KFGInvalidHandle;
        ERenderTargetLoadAction LoadAction = RTL_DontCare;
        ERenderTargetStoreAction StoreAction = RTS_DontCare;
        uint8 MipIndex = 0;
        int16 ArraySlice = -1;

        bool IsValid() const
        {
            return Texture != KFGInvalidHandle;
        }
    };

    struct DepthStencilEntry
    {
        FFGHandle Texture = KFGInvalidHandle;
        FFGHandle ResolveTexture = KFGInvalidHandle;
        ERenderTargetLoadAction DepthLoadAction = RTL_DontCare;
        ERenderTargetStoreAction DepthStoreAction = RTS_DontCare;
        ERenderTargetLoadAction StencilLoadAction = RTL_DontCare;
        ERenderTargetStoreAction StencilStoreAction = RTS_DontCare;

        bool IsValid() const
        {
            return Texture != KFGInvalidHandle;
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
        return *this;
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

    void SetMultiView(uint8 InMultiViewCount)
    {
        MultiViewCount = InMultiViewCount;
    }

    void InitRHI(class FFGPassResources& Resources);

    bool IsRenderPass() const { return bIsRenderPass; }
    FRHIRenderPass* GetRenderPassRHI() const { return RenderPassRHI.get(); }
    const FFGRenderTargets& GetRenderTargets() const { return RenderTargets;  } 
    const FFGHandle GetColorTexture(uint8 Index) const;
    const FFGHandle GetDepthStencilTexture() const;

protected:
    FFGRenderTargets RenderTargets;
    FRHIRenderPassRef RenderPassRHI = nullptr;
    uint8 MultiViewCount = 0;
    bool bIsRenderPass = false;

    friend class FFGUtils;
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
struct FFGExecuteData
{
    FFGExecuteData(FRHICommandList& InRHICmdList, FSceneProxy* InSceneProxy)
        : RHICmdList(InRHICmdList)
        , SceneProxy(InSceneProxy)
    {
    }

    FRHICommandList& RHICmdList;
    FSceneProxy* SceneProxy = nullptr;
};

struct FFGSetupData
{
    uint32 Width = 0;
    uint32 Height = 0;
    FSceneProxy* SceneProxy = nullptr;
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

    void Execute(FFGExecuteData& ContextData)
    {
        Graph->execute(&ContextData);
    }

    template<typename PassDataType>
    using SetupFuncType = std::function<void(FFGBuilder&, PassDataType&)>;

    template<typename PassDataType>
    using ExecuteFuncType = std::function<void(const PassDataType&, FFGPassResources&, FFGExecuteData&)>;

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
                FFGExecuteData* ContextData = static_cast<FFGExecuteData*>(Context);
                FFGPassData* PassData = (FFGPassData*)(&Data);

                REV_CORE_ASSERT(PassData !=  nullptr);

                if (PassData->IsRenderPass())
                    PassData->InitRHI(Resources);

                ContextData->RHICmdList.GetContext()->RHIBeginDebugLabel(InName, Math::FLinearColor(0.6f, 0.8f, 0.6f));

                Execute(Data, Resources, *ContextData);
                
                ContextData->RHICmdList.GetContext()->RHIEndDebugLabel();
            };

        auto& FGPass = Graph->addCallbackPass<PassDataType>(InName, std::move(SetupFuncImpl), std::move(ExecuteFuncImpl));
        if (bAddToBlackboard)
        {
            Blackboard->add<PassDataType>() = FGPass;
            LastPassData = Blackboard->try_get<PassDataType>();
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

    FFGPassData* GetLastPassData() const
    {
        return LastPassData;
    }


    friend std::ostream& operator<<(std::ostream&, const Rev::FFrameGraph&);

private:
    Scope<FrameGraph> Graph;
    Scope<FrameGraphBlackboard> Blackboard;
    FFGPassData* LastPassData;
};

class FFGUtils
{
public:
    static void RHIBeginPass(FRHICommandList& RHICmdList, const FFGPassData* PassData);
    static void RHIEndPass(FRHICommandList& RHICmdList, const FFGPassData* PassData);
};

}