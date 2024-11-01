#include "VulkanRenderPass.h"
#include "VulkanPixelFormat.h"
#include "VulkanTexture.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"
#include "Core/VulkanDefines.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanRenderPass::FVulkanRenderPass(const FRHIRenderPassDesc& InDesc)
	: FRHIRenderPass(InDesc)
{
	REV_CORE_ASSERT(mDesc.NumColorRenderTargets <= (uint32)RTA_MaxColorAttachments);
	Init();
}

FVulkanRenderPass::~FVulkanRenderPass()
{
	Release();
}

void FVulkanRenderPass::MarkFramebufferDirty()
{
	if (mNumAttachments > 0)
		bFramebufferDirty = true;
}

void FVulkanRenderPass::PrepareForDraw()
{
	if(mNumAttachments == 0)
		return;
	if(mFramebuffer && !bFramebufferDirty)
		return;
	bFramebufferDirty = false;
	if(mFramebuffer)
		vkDestroyFramebuffer(FVulkanDynamicRHI::GetDevice(), mFramebuffer, nullptr);

	bool bFirstColorTarget = true;

	std::array<VkImageView, RTA_MaxColorAttachments + 1> AttachmentViews;
	uint32 NumAttachmentViews = 0;
	for (uint32 i = 0; i < mDesc.NumColorRenderTargets; i++)
	{
		FVulkanTexture* ColorTarget = FVulkanTexture::Cast(mDesc.ColorRenderTargets[i].ColorTarget);
		AttachmentViews[NumAttachmentViews] = ColorTarget->GetImageView();
		NumAttachmentViews++;
		if (bFirstColorTarget)
		{
			mFrameWidth = ColorTarget->GetWidth();
			mFrameHeight = ColorTarget->GetHeight();
			bFirstColorTarget = false;
		}
	}
	if (mDesc.DepthStencilRenderTarget.DepthStencilTarget)
	{
		FVulkanTexture* DepthStencilTarget = FVulkanTexture::Cast(mDesc.DepthStencilRenderTarget.DepthStencilTarget);
		AttachmentViews[NumAttachmentViews] = DepthStencilTarget->GetImageView();
		NumAttachmentViews++;
	}
	
	VkFramebufferCreateInfo FramebufferInfo{};
	FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FramebufferInfo.renderPass = mRenderPass;
	FramebufferInfo.attachmentCount = NumAttachmentViews;
	FramebufferInfo.pAttachments = AttachmentViews.data();
	FramebufferInfo.width = mFrameWidth;
	FramebufferInfo.height = mFrameHeight;
	FramebufferInfo.layers = 1;

	REV_VK_CHECK_THROW(vkCreateFramebuffer(FVulkanDynamicRHI::GetDevice(), &FramebufferInfo, nullptr, &mFramebuffer), "failed to create framebuffer!");
}

void FVulkanRenderPass::Init()
{
	VkAttachmentDescription2 AttachmentDescs[RTA_MaxColorAttachments + 1];
	for (uint32 i = 0; i < mDesc.NumColorRenderTargets; i++)
	{
		const FRHIRenderPassDesc::FColorEntry& ColorEntry = mDesc.ColorRenderTargets[i];
		FVulkanTexture* ColorTarget = FVulkanTexture::Cast(ColorEntry.ColorTarget);
		REV_CORE_ASSERT(ColorTarget);

		EPixelFormat ColorFormat = ColorTarget->GetFormat();

		VkAttachmentDescription2& AttachmentDesc = AttachmentDescs[i];
		AttachmentDesc = {};
		AttachmentDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		AttachmentDesc.format = (VkFormat)GPixelFormats[ColorFormat].PlatformFormat;
		AttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDesc.loadOp = FVulkanEnum::Translate(ColorEntry.LoadOp);
		AttachmentDesc.storeOp = FVulkanEnum::Translate(ColorEntry.StoreOp);
		AttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDesc.initialLayout = ColorEntry.LoadOp == ALO_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		AttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		mClearValues[i] = ColorTarget->GetClearValue();
	}

	int32 DepthStencilIndex = -1;
	if (mDesc.DepthStencilRenderTarget.DepthStencilTarget)
	{
		FVulkanTexture* DepthStencilTarget = FVulkanTexture::Cast(mDesc.DepthStencilRenderTarget.DepthStencilTarget);
		EPixelFormat DepthFormat = DepthStencilTarget->GetFormat();
		REV_CORE_ASSERT(FPixelFormatInfo::HasDepth(DepthFormat));
		bool bHasStencil = FPixelFormatInfo::HasStencil(DepthFormat);
	
		DepthStencilIndex = mDesc.NumColorRenderTargets;

		VkAttachmentDescription2& AttachmentDesc = AttachmentDescs[DepthStencilIndex];
		AttachmentDesc = {};
		AttachmentDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		AttachmentDesc.format = (VkFormat)GPixelFormats[DepthFormat].PlatformFormat;
		AttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDesc.loadOp = FVulkanEnum::Translate(mDesc.DepthStencilRenderTarget.DepthLoadOp);
		AttachmentDesc.storeOp = FVulkanEnum::Translate(mDesc.DepthStencilRenderTarget.DepthStoreOp);
		AttachmentDesc.stencilLoadOp = bHasStencil ? FVulkanEnum::Translate(mDesc.DepthStencilRenderTarget.StencilLoadOp) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDesc.stencilStoreOp = bHasStencil ? FVulkanEnum::Translate(mDesc.DepthStencilRenderTarget.StencilStoreOp) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDesc.initialLayout = mDesc.DepthStencilRenderTarget.DepthLoadOp == ALO_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		AttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		mClearValues[DepthStencilIndex] = DepthStencilTarget->GetClearValue();
	}

	uint32 AttachmentCount = DepthStencilIndex >= 0 ? DepthStencilIndex + 1 : mDesc.NumColorRenderTargets;

	struct FSubpassAttachmentRef
	{
		//uint32 InputAttachmentCount;
		//VkAttachmentReference2 InputAttachments[RTA_MaxColorAttachments + 2];
		uint32 ColorAttachmentCount;
		VkAttachmentReference2 ColorAttachments[RTA_MaxColorAttachments];
		VkAttachmentReference2 ResolveAttachments[RTA_MaxColorAttachments];
		VkAttachmentReference2 DepthStencilAttachment;
	};


	VkSubpassDescription2 SubpassDescs[REV_VK_MAX_SUBPASSES];
	FSubpassAttachmentRef AttachmentRefs[REV_VK_MAX_SUBPASSES];
	uint32 NumSubpass = 0;
	{
		//Main subpass
		VkSubpassDescription2& SubpassDesc = SubpassDescs[NumSubpass];
		FSubpassAttachmentRef& AttachmentRef = AttachmentRefs[NumSubpass];
		NumSubpass++;

		bool bEnableResolve = false;
		AttachmentRef.ColorAttachmentCount = mDesc.NumColorRenderTargets;
		for (uint32 i = 0; i < mDesc.NumColorRenderTargets; i++)
		{
			VkAttachmentReference2& ColorRef = AttachmentRef.ColorAttachments[i];
			ColorRef = {};
			ColorRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			ColorRef.pNext = nullptr;
			ColorRef.attachment = i;
			ColorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			ColorRef.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			/*const FRHIRenderPassDesc::FColorEntry& ColorEntry = mDesc.ColorRenderTargets[i];
			if (ColorEntry.ResolveTarget)
			{
				VkAttachmentReference2& ResolveRef = AttachmentRef.ResolveAttachments[i];
				ResolveRef = {};
				ResolveRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
				ResolveRef.pNext = nullptr;
				ResolveRef.attachment = i + AttachmentRef.ColorAttachmentCount;
				ResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				ResolveRef.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

				bEnableResolve = true;
			}*/
		}

		bool bEnableDepthStencil = false;
		if (DepthStencilIndex >= 0)
		{
			VkAttachmentReference2& DepthStencilRef = AttachmentRef.DepthStencilAttachment;
			DepthStencilRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			DepthStencilRef.pNext = nullptr;
			DepthStencilRef.attachment = (uint32)DepthStencilIndex;
			DepthStencilRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			DepthStencilRef.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

			bEnableDepthStencil = true;
		}

		SubpassDesc = {};
		SubpassDesc.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
		SubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDesc.colorAttachmentCount = AttachmentRef.ColorAttachmentCount;
		SubpassDesc.pColorAttachments = AttachmentRef.ColorAttachments;
		SubpassDesc.pResolveAttachments = bEnableResolve ? AttachmentRef.ResolveAttachments : nullptr;
		SubpassDesc.pDepthStencilAttachment = bEnableDepthStencil ? &AttachmentRef.DepthStencilAttachment : nullptr;
		SubpassDesc.inputAttachmentCount = 0;
		SubpassDesc.pInputAttachments = nullptr;
	}


	VkRenderPassCreateInfo2 RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
	RenderPassInfo.attachmentCount = AttachmentCount;
	RenderPassInfo.pAttachments = AttachmentDescs;
	RenderPassInfo.subpassCount = NumSubpass;
	RenderPassInfo.pSubpasses = SubpassDescs;

	REV_VK_CHECK_THROW(vkCreateRenderPass2(FVulkanDynamicRHI::GetDevice(), &RenderPassInfo, nullptr, &mRenderPass), "failed to create render pass!");

	mNumAttachments = AttachmentCount;
}

void FVulkanRenderPass::Release()
{
	if (mFramebuffer)
	{
		vkDestroyFramebuffer(FVulkanDynamicRHI::GetDevice(), mFramebuffer, nullptr);
	}
	vkDestroyRenderPass(FVulkanDynamicRHI::GetDevice(), mRenderPass, nullptr);
}


//DynamicRHI
Ref<FRHIRenderPass> FVulkanDynamicRHI::RHICreateRenderPass(const FRHIRenderPassDesc& InDesc)
{
	return CreateRef<FVulkanRenderPass>(InDesc);
}

}
