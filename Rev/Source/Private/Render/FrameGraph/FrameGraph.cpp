#include "Rev/Render/FrameGraph.h"

namespace Rev
{

void FFGPassData::SetColorTarget(uint8 InIndex, FFGHandle InTexture, FFGHandle InTextureResolve, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction, uint8 InMipIndex, int16 InArraySlice)
{
    REV_CORE_ASSERT(InIndex < RTA_MaxColorAttachments);
    RenderTargets.ColorTargets[InIndex] = { InTexture, InTextureResolve, InLoadAction, InStoreAction, InMipIndex, InArraySlice };
    bIsRenderPass = true;
}

void FFGPassData::SetDepthStencilTarget(FFGHandle InTexture, FFGHandle InTextureResolve, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetStoreAction InStencilStoreAction)
{
    RenderTargets.DepthStencilTarget = { InTexture, InTextureResolve, InDepthLoadAction, InDepthStoreAction, InStencilLoadAction, InStencilStoreAction };
    bIsRenderPass = true;
}

void FFGPassData::InitRHI(FFGPassResources& Resources)
{
    if (RenderPassRHI)
        return;

    FRHIRenderPassDesc Desc;
    Desc.NumColorRenderTargets = RenderTargets.GetNumColorTargets();
    for (uint32 i = 0; i < Desc.NumColorRenderTargets; i++)
    {
        FRHITexture* Texture = Resources.Get<FFGTexture>(RenderTargets.ColorTargets[i].Texture).GetTextureRHI();
        FRHITexture* TextureResolve = RenderTargets.ColorTargets[i].ResolveTexture != KFGInvalidHandle ? Resources.Get<FFGTexture>(RenderTargets.ColorTargets[i].ResolveTexture).GetTextureRHI() : nullptr;

        Desc.ColorRenderTargets[i] = {
            Texture,
            TextureResolve,
            RenderTargets.ColorTargets[i].ArraySlice,
            RenderTargets.ColorTargets[i].MipIndex,
            RenderTargets.ColorTargets[i].LoadAction,
            RenderTargets.ColorTargets[i].StoreAction
        };
    }

    if (RenderTargets.DepthStencilTarget.IsValid())
    {
        FRHITexture* Texture = Resources.Get<FFGTexture>(RenderTargets.DepthStencilTarget.Texture).GetTextureRHI();
        FRHITexture* TextureResolve = RenderTargets.DepthStencilTarget.ResolveTexture != KFGInvalidHandle ? Resources.Get<FFGTexture>(RenderTargets.DepthStencilTarget.ResolveTexture).GetTextureRHI() : nullptr;

        Desc.DepthStencilRenderTarget = {
            Texture,
            TextureResolve,
            RenderTargets.DepthStencilTarget.DepthLoadAction,
            RenderTargets.DepthStencilTarget.DepthStoreAction,
            RenderTargets.DepthStencilTarget.StencilLoadAction,
            RenderTargets.DepthStencilTarget.StencilStoreAction
        };
    }

    RenderPassRHI = GDynamicRHI->RHICreateRenderPass(Desc);
}

const FFGHandle FFGPassData::GetColorTexture(uint8 Index) const
{
    REV_CORE_ASSERT(Index < RTA_MaxColorAttachments);

    if (RenderTargets.ColorTargets[Index].ResolveTexture != KFGInvalidHandle)
    {
        return RenderTargets.ColorTargets[Index].ResolveTexture;
    }

    return RenderTargets.ColorTargets[Index].Texture;
}

const FFGHandle FFGPassData::GetDepthStencilTexture() const
{
    if (RenderTargets.DepthStencilTarget.ResolveTexture != KFGInvalidHandle)
    {
        return RenderTargets.DepthStencilTarget.ResolveTexture;
    }

    return RenderTargets.DepthStencilTarget.Texture;
}

std::ostream& operator<<(std::ostream& OStream, const Rev::FFrameGraph& FG)
{
    return OStream << *FG.Graph;
}

}
