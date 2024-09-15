#include "VulkanRenderTarget.h"
#include "VulkanCore.h"
#include "Core/VulkanDefines.h"

namespace Rev
{

FVulkanRenderTarget::FVulkanRenderTarget(const FRenderTargetDesc& InDesc)
	: FRHIRenderTarget(InDesc)
{

}

FVulkanRenderTarget::~FVulkanRenderTarget()
{
}

void FVulkanRenderTarget::ResizeTargets(uint16 InWidth, uint16 InHeight)
{
    if (IsEmptyTarget())
        return;

    if (InWidth == 0 || InHeight == 0 || InWidth > REV_VK_RENDERTARGET_SIZE_MAX || InHeight > REV_VK_RENDERTARGET_SIZE_MAX)
    {
        REV_CORE_WARN("Attempted to rezize framebuffer to {0}, {1} failed", InWidth, InHeight);
        return;
    }
    if (mDesc.Width == InWidth && mDesc.Height == InHeight)
        return;

    mDesc.Width = InWidth;
    mDesc.Height = InHeight;
    ReleaseResource();
    CreateResource();
}

const Ref<FRHITexture> FVulkanRenderTarget::GetTargetTexture(ERenderTargetAttachment Index) const
{
    if (Index == RTA_DepthStencilAttachment)
    {
        return mDepthStencilAttachment.Texture;
    }
    else if (Index >= 0 && Index < RTA_MaxColorAttachments)
    {
        return mColorAttachments[Index].Texture;
    }
    return nullptr;
}

bool FVulkanRenderTarget::IsEmptyTarget() const
{
    return mDesc.Dimension == ERenderTargetDimension::RenderTargetEmpty;
}

void FVulkanRenderTarget::CreateResource()
{
    if (IsEmptyTarget())
        return;

    std::vector<VkImageView> Attachments;
    for (uint32 i = 0; i < mDesc.NumColorTargets; i++)
    {
        if (auto ColorTexture = CreateColorTexture(mDesc.ColorTargets[i]); ColorTexture)
        {
            Attachments.push_back(ColorTexture->GetImageView());
            mColorAttachments[i].Texture = std::move(ColorTexture);
        }
    }
    if (auto DepthTexture = CreateDepthStencilTexture(mDesc.DepthStencilTarget); DepthTexture)
    {
        Attachments.push_back(DepthTexture->GetImageView());
        mDepthStencilAttachment.Texture = std::move(DepthTexture);
    }

    VkFramebufferCreateInfo FramebufferInfo{};
    FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    FramebufferInfo.renderPass = renderPass;
    FramebufferInfo.attachmentCount = (uint32_t)Attachments.size();
    FramebufferInfo.pAttachments = Attachments.data();
    FramebufferInfo.width = (uint32_t)mDesc.Width;
    FramebufferInfo.height = (uint32_t)mDesc.Height;
    FramebufferInfo.layers = 1;

    if (vkCreateFramebuffer(FVulkanCore::GetDevice(), &FramebufferInfo, nullptr, &mFramebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

void FVulkanRenderTarget::ReleaseResource()
{
    vkDestroyFramebuffer(FVulkanCore::GetDevice(), mFramebuffer, nullptr);
    if (!IsEmptyTarget())
    {
        for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
        {
            //mColorAttachments[i].MipLevel = 0;
            //mColorAttachments[i].ArrayIndex = -1;
            mColorAttachments[i].Texture.reset();
        }
        //mDepthStencilAttachment.MipLevel = 0;
        //mDepthStencilAttachment.ArrayIndex = -1;
        mDepthStencilAttachment.Texture.reset();
    }
}

Ref<FVulkanTexture> FVulkanRenderTarget::CreateColorTexture(const FColorTargetDesc& InDesc)
{
    REV_CORE_ASSERT(!IsEmptyTarget());
    if (InDesc.Format == PF_Unknown)
        return nullptr;
    FTextureDesc TexDesc((ETextureDimension)mDesc.Dimension, InDesc.Format, false, mDesc.Width, mDesc.Height, 0, mDesc.ArraySize, InDesc.ClearColor, 1, mDesc.NumSamples);
    return CreateVulkanTexture(TexDesc, FSamplerDesc());
}

Ref<FVulkanTexture> FVulkanRenderTarget::CreateDepthStencilTexture(const FDepthStencilTargetDesc& InDesc)
{
    REV_CORE_ASSERT(!IsEmptyTarget());
    if (InDesc.Format != PF_ShadowDepth && InDesc.Format != PF_DepthStencil)
        return nullptr;
    FTextureDesc TexDesc((ETextureDimension)mDesc.Dimension, InDesc.Format, false, mDesc.Width, mDesc.Height, 0, mDesc.ArraySize, { InDesc.ClearDepth, InDesc.ClearStencil }, 1, mDesc.NumSamples);
    return CreateVulkanTexture(TexDesc, FSamplerDesc());
}

}

