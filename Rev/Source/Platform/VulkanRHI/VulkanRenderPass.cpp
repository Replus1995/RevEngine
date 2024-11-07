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
	REV_CORE_ASSERT(PassDesc.NumColorRenderTargets <= (uint32)RTA_MaxColorAttachments);
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

	VkImageView AttachmentViews[(RTA_MaxColorAttachments + 1) * 2];
	uint32 NumAttachmentViews = 0;
	for (uint32 i = 0; i < PassDesc.NumColorRenderTargets; i++)
	{
		FVulkanTexture* ColorTarget = FVulkanTexture::Cast(PassDesc.ColorRenderTargets[i].ColorTarget);
		AttachmentViews[NumAttachmentViews] = ColorTarget->GetImageView();
		NumAttachmentViews++;

		if (PassDesc.ColorRenderTargets[i].ResolveTarget)
		{
			FVulkanTexture* ResolveTarget = FVulkanTexture::Cast(PassDesc.ColorRenderTargets[i].ResolveTarget);
			AttachmentViews[NumAttachmentViews] = ResolveTarget->GetImageView();
			NumAttachmentViews++;
		}


		if (bFirstColorTarget)
		{
			mFrameWidth = ColorTarget->GetWidth();
			mFrameHeight = ColorTarget->GetHeight();
			bFirstColorTarget = false;
		}
	}
	if (PassDesc.DepthStencilRenderTarget.DepthStencilTarget)
	{
		FVulkanTexture* DepthStencilTarget = FVulkanTexture::Cast(PassDesc.DepthStencilRenderTarget.DepthStencilTarget);
		AttachmentViews[NumAttachmentViews] = DepthStencilTarget->GetImageView();
		NumAttachmentViews++;

		if (PassDesc.DepthStencilRenderTarget.ResolveTarget)
		{
			FVulkanTexture* ResolveTarget = FVulkanTexture::Cast(PassDesc.DepthStencilRenderTarget.ResolveTarget);
			AttachmentViews[NumAttachmentViews] = ResolveTarget->GetImageView();
			NumAttachmentViews++;
		}
	}
	
	VkFramebufferCreateInfo FramebufferInfo{};
	FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FramebufferInfo.renderPass = mRenderPass;
	FramebufferInfo.attachmentCount = NumAttachmentViews;
	FramebufferInfo.pAttachments = AttachmentViews;
	FramebufferInfo.width = mFrameWidth;
	FramebufferInfo.height = mFrameHeight;
	FramebufferInfo.layers = 1;

	REV_VK_CHECK_THROW(vkCreateFramebuffer(FVulkanDynamicRHI::GetDevice(), &FramebufferInfo, nullptr, &mFramebuffer), "failed to create framebuffer!");
}

void FVulkanRenderPass::Init()
{
	struct FAttachmentIndices
	{
		uint32 Colors[RTA_MaxColorAttachments];
		uint32 ColorResolves[RTA_MaxColorAttachments];
		uint32 DepthStencil;
		uint32 DepthStencilResolve;
	};

	constexpr uint32 MaxNumAttachments = (RTA_MaxColorAttachments + 1) * 2;
	uint32 NumAttachmentDesc = 0;
	FAttachmentIndices AttachmentIndices;

	VkAttachmentDescription2 AttachmentDescs[MaxNumAttachments];
	for (uint32 i = 0; i < PassDesc.NumColorRenderTargets; i++)
	{
		const FRHIRenderPassDesc::FColorEntry& ColorEntry = PassDesc.ColorRenderTargets[i];
		FVulkanTexture* ColorTarget = FVulkanTexture::Cast(ColorEntry.ColorTarget);
		REV_CORE_ASSERT(ColorTarget);

		VkFormat ColorFormat = (VkFormat)GPixelFormats[ColorTarget->GetFormat()].PlatformFormat;

		VkAttachmentDescription2& ColorDesc = AttachmentDescs[NumAttachmentDesc];
		ColorDesc = {};
		ColorDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		ColorDesc.format = ColorFormat;
		ColorDesc.samples = ColorTarget->GetSamplerCount();
		ColorDesc.loadOp = FVulkanEnum::Translate(ColorEntry.LoadOp);
		ColorDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorDesc.initialLayout = ColorEntry.LoadOp == ALO_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		ColorDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		if (ColorDesc.samples == VK_SAMPLE_COUNT_1_BIT)
		{
			ColorDesc.storeOp = FVulkanEnum::Translate(ColorEntry.StoreOp);
		}
		else
		{
			ColorDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		mClearValues[NumAttachmentDesc] = ColorTarget->GetClearValue();
		AttachmentIndices.Colors[i] = NumAttachmentDesc;
		NumAttachmentDesc++;

		if (ColorEntry.ResolveTarget)
		{
			FVulkanTexture* ResolveTarget = FVulkanTexture::Cast(ColorEntry.ResolveTarget);
			VkAttachmentDescription2& ResolveDesc = AttachmentDescs[NumAttachmentDesc];
			ResolveDesc = {};
			ResolveDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
			ResolveDesc.format = ColorFormat;
			ResolveDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			ResolveDesc.loadOp = FVulkanEnum::Translate(ColorEntry.LoadOp);
			ResolveDesc.storeOp = FVulkanEnum::Translate(ColorEntry.StoreOp);
			ResolveDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			ResolveDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			ResolveDesc.initialLayout = ColorEntry.LoadOp == ALO_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			ResolveDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			mClearValues[NumAttachmentDesc] = ResolveTarget->GetClearValue();
			AttachmentIndices.ColorResolves[i] = NumAttachmentDesc;
			NumAttachmentDesc++;
		}
	}

	if (PassDesc.DepthStencilRenderTarget.DepthStencilTarget)
	{
		const FRHIRenderPassDesc::FDepthStencilEntry& DepthStencilEntry = PassDesc.DepthStencilRenderTarget;
		FVulkanTexture* DepthStencilTarget = FVulkanTexture::Cast(DepthStencilEntry.DepthStencilTarget);
		EPixelFormat DepthFormat = DepthStencilTarget->GetFormat();
		REV_CORE_ASSERT(FPixelFormatInfo::HasDepth(DepthFormat));
		bool bHasStencil = FPixelFormatInfo::HasStencil(DepthFormat);

		VkAttachmentDescription2& DepthStencilDesc = AttachmentDescs[NumAttachmentDesc];
		DepthStencilDesc = {};
		DepthStencilDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		DepthStencilDesc.format = (VkFormat)GPixelFormats[DepthFormat].PlatformFormat;
		DepthStencilDesc.samples = DepthStencilTarget->GetSamplerCount();
		DepthStencilDesc.loadOp = FVulkanEnum::Translate(DepthStencilEntry.DepthLoadOp);
		DepthStencilDesc.stencilLoadOp = bHasStencil ? FVulkanEnum::Translate(DepthStencilEntry.StencilLoadOp) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		DepthStencilDesc.initialLayout = DepthStencilEntry.DepthLoadOp == ALO_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		DepthStencilDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		if (DepthStencilDesc.samples == VK_SAMPLE_COUNT_1_BIT)
		{
			DepthStencilDesc.storeOp = FVulkanEnum::Translate(DepthStencilEntry.DepthStoreOp);
			DepthStencilDesc.stencilStoreOp = bHasStencil ? FVulkanEnum::Translate(DepthStencilEntry.StencilStoreOp) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
		else
		{
			DepthStencilDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			DepthStencilDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
		
		mClearValues[NumAttachmentDesc] = DepthStencilTarget->GetClearValue();
		AttachmentIndices.DepthStencil = NumAttachmentDesc;
		NumAttachmentDesc++;

		if (DepthStencilEntry.ResolveTarget)
		{
			FVulkanTexture* ResolveTarget = FVulkanTexture::Cast(DepthStencilEntry.ResolveTarget);
			VkAttachmentDescription2& ResolveDesc = AttachmentDescs[NumAttachmentDesc];
			ResolveDesc = {};
			ResolveDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
			ResolveDesc.format = (VkFormat)GPixelFormats[DepthFormat].PlatformFormat;
			ResolveDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			ResolveDesc.loadOp = FVulkanEnum::Translate(DepthStencilEntry.DepthLoadOp);
			ResolveDesc.storeOp = FVulkanEnum::Translate(DepthStencilEntry.DepthStoreOp);
			ResolveDesc.stencilLoadOp = bHasStencil ? FVulkanEnum::Translate(DepthStencilEntry.StencilLoadOp) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			ResolveDesc.stencilStoreOp = bHasStencil ? FVulkanEnum::Translate(DepthStencilEntry.StencilStoreOp) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
			ResolveDesc.initialLayout = DepthStencilEntry.DepthLoadOp == ALO_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			ResolveDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			mClearValues[NumAttachmentDesc] = ResolveTarget->GetClearValue();
			AttachmentIndices.DepthStencilResolve = NumAttachmentDesc;
			NumAttachmentDesc++;
		}
	}

	struct FSubpassAttachmentRef
	{
		//uint32 InputAttachmentCount;
		//VkAttachmentReference2 InputAttachments[RTA_MaxColorAttachments + 2];
		uint32 NumColorAttachments;
		VkAttachmentReference2 ColorAttachments[RTA_MaxColorAttachments];
		VkAttachmentReference2 ColorResolveAttachments[RTA_MaxColorAttachments];
		VkAttachmentReference2 DepthStencilAttachment;
		VkAttachmentReference2 DepthStencilResolveAttachment;
	};

	VkSubpassDescription2 SubpassDescs[REV_VK_MAX_SUBPASSES];
	VkSubpassDescriptionDepthStencilResolve SubpassDescDepthStencilResolves[REV_VK_MAX_SUBPASSES];
	FSubpassAttachmentRef AttachmentRefs[REV_VK_MAX_SUBPASSES];

	uint32 NumSubpass = 0;
	{
		uint32 NumAttachmentRefs = 0;
		bool bEnableColorResolve = false;
		bool bEnableDepthStencil = false;
		bool bEnableDepthStencilResolve = false;


		//Main subpass
		VkSubpassDescription2& SubpassDesc = SubpassDescs[NumSubpass];
		FSubpassAttachmentRef& AttachmentRef = AttachmentRefs[NumSubpass];
		NumSubpass++;

		
		AttachmentRef.NumColorAttachments = PassDesc.NumColorRenderTargets;
		for (uint32 i = 0; i < AttachmentRef.NumColorAttachments; i++)
		{
			const FRHIRenderPassDesc::FColorEntry& ColorEntry = PassDesc.ColorRenderTargets[i];
			VkAttachmentReference2& ColorRef = AttachmentRef.ColorAttachments[i];
			ColorRef = {};
			ColorRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			ColorRef.pNext = nullptr;
			ColorRef.attachment = AttachmentIndices.Colors[i];
			ColorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			ColorRef.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			if (ColorEntry.ResolveTarget)
			{
				VkAttachmentReference2& ResolveRef = AttachmentRef.ColorResolveAttachments[i];
				ResolveRef = {};
				ResolveRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
				ResolveRef.pNext = nullptr;
				ResolveRef.attachment = AttachmentIndices.ColorResolves[i];
				ResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				ResolveRef.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

				bEnableColorResolve = true;
			}
		}

		if (PassDesc.DepthStencilRenderTarget.DepthStencilTarget)
		{
			VkAttachmentReference2& DepthStencilRef = AttachmentRef.DepthStencilAttachment;
			DepthStencilRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			DepthStencilRef.pNext = nullptr;
			DepthStencilRef.attachment = AttachmentIndices.DepthStencil;
			DepthStencilRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			DepthStencilRef.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

			bEnableDepthStencil = true;

			if (PassDesc.DepthStencilRenderTarget.ResolveTarget)
			{

				VkAttachmentReference2& DepthStencilRef = AttachmentRef.DepthStencilResolveAttachment;
				DepthStencilRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
				DepthStencilRef.pNext = nullptr;
				DepthStencilRef.attachment = AttachmentIndices.DepthStencilResolve;
				DepthStencilRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				DepthStencilRef.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

				bEnableDepthStencilResolve = true;
			}
		}

		SubpassDesc = {};
		SubpassDesc.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
		SubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDesc.colorAttachmentCount = AttachmentRef.NumColorAttachments;
		SubpassDesc.pColorAttachments = AttachmentRef.ColorAttachments;
		SubpassDesc.pResolveAttachments = bEnableColorResolve ? AttachmentRef.ColorResolveAttachments : nullptr;
		SubpassDesc.pDepthStencilAttachment = bEnableDepthStencil ? &AttachmentRef.DepthStencilAttachment : nullptr;
		SubpassDesc.inputAttachmentCount = 0;
		SubpassDesc.pInputAttachments = nullptr;

		if (bEnableDepthStencilResolve)
		{
			VkSubpassDescriptionDepthStencilResolve SubpassDescDepthStencilResolve = SubpassDescDepthStencilResolves[NumSubpass];
			SubpassDescDepthStencilResolve = {};
			SubpassDescDepthStencilResolve.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE;
			SubpassDescDepthStencilResolve.pNext = nullptr;
			SubpassDescDepthStencilResolve.depthResolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
			SubpassDescDepthStencilResolve.stencilResolveMode = VK_RESOLVE_MODE_MIN_BIT;
			SubpassDescDepthStencilResolve.pDepthStencilResolveAttachment = &AttachmentRef.DepthStencilResolveAttachment;

			SubpassDesc.pNext = &SubpassDescDepthStencilResolve;
		}
	}


	VkRenderPassCreateInfo2 RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
	RenderPassInfo.attachmentCount = NumAttachmentDesc;
	RenderPassInfo.pAttachments = AttachmentDescs;
	RenderPassInfo.subpassCount = NumSubpass;
	RenderPassInfo.pSubpasses = SubpassDescs;

	REV_VK_CHECK_THROW(vkCreateRenderPass2(FVulkanDynamicRHI::GetDevice(), &RenderPassInfo, nullptr, &mRenderPass), "failed to create render pass!");

	mNumAttachments = NumAttachmentDesc;
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
