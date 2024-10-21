#include "VulkanRenderTarget.h"
#include "VulkanTexture2D.h"
#include "VulkanDynamicRHI.h"


namespace Rev
{

FVulkanRenderTarget::FVulkanRenderTarget(const FRenderTargetDesc& InDesc)
	: FRHIRenderTarget(InDesc)
{

    if (!IsEmptyTarget())
    {
        REV_CORE_ASSERT(InDesc.Width > 0 && InDesc.Width <= REV_VK_RENDERTARGET_SIZE_MAX, "Invalid render target width");
        REV_CORE_ASSERT(InDesc.Height > 0 && InDesc.Height <= REV_VK_RENDERTARGET_SIZE_MAX, "Invalid render target height");
    }
}

FVulkanRenderTarget::~FVulkanRenderTarget()
{
    ReleaseResource();
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
    mNeedResize = true;
}

const FRHITexture* FVulkanRenderTarget::GetTargetTexture(ERenderTargetAttachment Index) const
{
    if (Index == RTA_DepthStencilAttachment)
    {
        return mDepthStencilAttachment.Texture.get();
    }
    else if (Index >= 0 && Index < RTA_MaxColorAttachments)
    {
        return mColorAttachments[Index].Texture.get();
    }
    return nullptr;
}

void FVulkanRenderTarget::FlushResource(VkRenderPass InRenderPass)
{
    bool bNeedCreateFramebuffer = false;

    if (mNeedResize)
    {
        mNeedResize = false;
        bNeedCreateFramebuffer = true;
    }

    if (InRenderPass != mRenderPassCache)
    {
        mRenderPassCache = InRenderPass;
        bNeedCreateFramebuffer = true;
    }

    if (bNeedCreateFramebuffer)
    {
        ReleaseResource();
        CreateTextures();
        CreateFramebuffer(InRenderPass);
    }
}

bool FVulkanRenderTarget::IsEmptyTarget() const
{
    return mDesc.Dimension == ERenderTargetDimension::RenderTargetEmpty;
}

void FVulkanRenderTarget::CreateTextures()
{
    if (IsEmptyTarget())
        return;

    mNumClearValues = 0;
    for (uint32 i = 0; i < mDesc.NumColorTargets; i++)
    {
        if (auto ColorTexture = CreateColorTexture(mDesc.ColorTargets[i]); ColorTexture)
        {
            mColorAttachments[i].Texture = std::move(ColorTexture);
            mClearValues[mNumClearValues] = mColorAttachments[i].Texture->GetClearValue();
            mNumClearValues++;
        }
    }
    if (auto DepthTexture = CreateDepthStencilTexture(mDesc.DepthStencilTarget); DepthTexture)
    {
        mDepthStencilAttachment.Texture = std::move(DepthTexture);
        mClearValues[mNumClearValues] = mDepthStencilAttachment.Texture->GetClearValue();
        mNumClearValues++;
    }
}

void FVulkanRenderTarget::CreateFramebuffer(VkRenderPass InRenderPass)
{
    std::array<VkImageView, RTA_MaxColorAttachments + 1> Attachments;
    uint32 AttachmentCount = 0;
    for (uint32 i = 0; i < mDesc.NumColorTargets; i++)
    {
        Attachments[AttachmentCount] = mColorAttachments[i].Texture->GetImageView();
        AttachmentCount++;
    }
    if (mDepthStencilAttachment.Texture)
    {
        Attachments[AttachmentCount] = mDepthStencilAttachment.Texture->GetImageView();
        AttachmentCount++;
    }

    VkFramebufferCreateInfo FramebufferInfo{};
    FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    FramebufferInfo.renderPass = InRenderPass;
    FramebufferInfo.attachmentCount = (uint32_t)AttachmentCount;
    FramebufferInfo.pAttachments = Attachments.data();
    FramebufferInfo.width = (uint32_t)mDesc.Width;
    FramebufferInfo.height = (uint32_t)mDesc.Height;
    FramebufferInfo.layers = 1;

    if (vkCreateFramebuffer(FVulkanDynamicRHI::GetDevice(), &FramebufferInfo, nullptr, &mFramebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

void FVulkanRenderTarget::ReleaseResource()
{
    if (IsEmptyTarget())
        return;

    for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
    {
        //mColorAttachments[i].MipLevel = 0;
        //mColorAttachments[i].ArrayIndex = -1;
        mColorAttachments[i].Texture.reset();
    }
    //mDepthStencilAttachment.MipLevel = 0;
    //mDepthStencilAttachment.ArrayIndex = -1;
    mDepthStencilAttachment.Texture.reset();

    if(mFramebuffer)
        vkDestroyFramebuffer(FVulkanDynamicRHI::GetDevice(), mFramebuffer, nullptr);
}

Ref<FVulkanTexture> FVulkanRenderTarget::CreateColorTexture(const FColorTargetDesc& InDesc)
{
    REV_CORE_ASSERT(!IsEmptyTarget());
    if (InDesc.Format == PF_Unknown)
        return nullptr;
    FTextureDesc TexDesc((ETextureDimension)mDesc.Dimension, InDesc.Format);
    TexDesc.SetExtent(mDesc.Width, mDesc.Height, 1)
        .SetArraySize(mDesc.ArraySize)
        .SetClearColor(InDesc.ClearColor)
        .SetNumSamples(mDesc.NumSamples);

    VkImageUsageFlags UsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    return CreateRef<FVulkanTexture2D>(TexDesc, UsageFlags);
}

Ref<FVulkanTexture> FVulkanRenderTarget::CreateDepthStencilTexture(const FDepthStencilTargetDesc& InDesc)
{
    REV_CORE_ASSERT(!IsEmptyTarget());
    if (InDesc.Format != PF_ShadowDepth && InDesc.Format != PF_DepthStencil)
        return nullptr;
    FTextureDesc TexDesc((ETextureDimension)mDesc.Dimension, InDesc.Format);
    TexDesc.SetExtent(mDesc.Width, mDesc.Height, 1)
        .SetArraySize(mDesc.ArraySize)
        .SetClearColor({ InDesc.ClearDepth, InDesc.ClearStencil })
        .SetNumSamples(mDesc.NumSamples);

    VkImageUsageFlags UsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    return CreateRef<FVulkanTexture2D>(TexDesc, UsageFlags);
}

}

