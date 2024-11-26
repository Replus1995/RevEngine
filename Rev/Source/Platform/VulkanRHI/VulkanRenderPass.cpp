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

void FVulkanRenderPass::PrepareForDraw()
{
	REV_CORE_ASSERT(NumAttachments > 0);

	if(Framebuffer)
		return;
	/*if(mFramebuffer)
		vkDestroyFramebuffer(FVulkanDynamicRHI::GetDevice(), mFramebuffer, nullptr);*/

	bool bFirstTarget = true;

	memset(&FrameImageViews, 0, sizeof(FrameImageViews));

	uint32 NumAttachmentViews = 0;
	for (uint32 i = 0; i < PassDesc.NumColorRenderTargets; i++)
	{
		FVulkanTexture* ColorTarget = FVulkanTexture::Cast(PassDesc.ColorRenderTargets[i].ColorTarget);
		FrameImageViews[NumAttachmentViews] = CreateImageView(ColorTarget, PassDesc.ColorRenderTargets[i].ArraySlice, PassDesc.ColorRenderTargets[i].MipIndex);
		NumAttachmentViews++;

		if (PassDesc.ColorRenderTargets[i].ResolveTarget)
		{
			FVulkanTexture* ResolveTarget = FVulkanTexture::Cast(PassDesc.ColorRenderTargets[i].ResolveTarget);
			FrameImageViews[NumAttachmentViews] = CreateImageView(ResolveTarget, PassDesc.ColorRenderTargets[i].ArraySlice, PassDesc.ColorRenderTargets[i].MipIndex);
			NumAttachmentViews++;
		}


		if (bFirstTarget)
		{
			FrameWidth = ColorTarget->GetWidth();
			FrameHeight = ColorTarget->GetHeight();
			bFirstTarget = false;
		}
	}
	if (PassDesc.DepthStencilRenderTarget.DepthStencilTarget)
	{
		FVulkanTexture* DepthStencilTarget = FVulkanTexture::Cast(PassDesc.DepthStencilRenderTarget.DepthStencilTarget);
		FrameImageViews[NumAttachmentViews] = CreateImageView(DepthStencilTarget, -1, 0);
		NumAttachmentViews++;

		if (PassDesc.DepthStencilRenderTarget.ResolveTarget)
		{
			FVulkanTexture* ResolveTarget = FVulkanTexture::Cast(PassDesc.DepthStencilRenderTarget.ResolveTarget);
			FrameImageViews[NumAttachmentViews] = CreateImageView(ResolveTarget, -1, 0);
			NumAttachmentViews++;
		}

		if (bFirstTarget)
		{
			FrameWidth = DepthStencilTarget->GetWidth();
			FrameHeight = DepthStencilTarget->GetHeight();
			bFirstTarget = false;
		}
	}
	
	VkFramebufferCreateInfo FramebufferInfo{};
	FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FramebufferInfo.renderPass = RenderPass;
	FramebufferInfo.attachmentCount = NumAttachmentViews;
	FramebufferInfo.pAttachments = FrameImageViews;
	FramebufferInfo.width = FrameWidth;
	FramebufferInfo.height = FrameHeight;
	FramebufferInfo.layers = 1;

	REV_VK_CHECK_THROW(vkCreateFramebuffer(FVulkanDynamicRHI::GetDevice(), &FramebufferInfo, nullptr, &Framebuffer), "failed to create framebuffer!");
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
		ColorDesc.loadOp = FVulkanEnum::Translate(ColorEntry.LoadAction);
		ColorDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorDesc.initialLayout = ColorEntry.LoadAction == RTL_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		ColorDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		if (ColorDesc.samples == VK_SAMPLE_COUNT_1_BIT)
		{
			ColorDesc.storeOp = FVulkanEnum::Translate(ColorEntry.StoreAction);
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
			ResolveDesc.loadOp = FVulkanEnum::Translate(ColorEntry.LoadAction);
			ResolveDesc.storeOp = FVulkanEnum::Translate(ColorEntry.StoreAction);
			ResolveDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			ResolveDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			ResolveDesc.initialLayout = ColorEntry.LoadAction == RTL_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
		DepthStencilDesc.loadOp = FVulkanEnum::Translate(DepthStencilEntry.DepthLoadAction);
		DepthStencilDesc.stencilLoadOp = bHasStencil ? FVulkanEnum::Translate(DepthStencilEntry.StencilLoadAction) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		DepthStencilDesc.initialLayout = DepthStencilEntry.DepthLoadAction == RTL_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		DepthStencilDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		if (DepthStencilDesc.samples == VK_SAMPLE_COUNT_1_BIT)
		{
			DepthStencilDesc.storeOp = FVulkanEnum::Translate(DepthStencilEntry.DepthStoreAction);
			DepthStencilDesc.stencilStoreOp = bHasStencil ? FVulkanEnum::Translate(DepthStencilEntry.StencilStoreAction) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
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
			ResolveDesc.loadOp = FVulkanEnum::Translate(DepthStencilEntry.DepthLoadAction);
			ResolveDesc.storeOp = FVulkanEnum::Translate(DepthStencilEntry.DepthStoreAction);
			ResolveDesc.stencilLoadOp = bHasStencil ? FVulkanEnum::Translate(DepthStencilEntry.StencilLoadAction) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			ResolveDesc.stencilStoreOp = bHasStencil ? FVulkanEnum::Translate(DepthStencilEntry.StencilStoreAction) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
			ResolveDesc.initialLayout = DepthStencilEntry.DepthLoadAction == RTL_DontCare ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
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

	uint32 MultiViewMask = GetMultiViewMask(PassDesc.MultiViewCount);

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
		SubpassDesc.viewMask = MultiViewMask;

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

	if (MultiViewMask > 0)
	{
		RenderPassInfo.correlatedViewMaskCount = 1;
		RenderPassInfo.pCorrelatedViewMasks = &MultiViewMask;
	}

	REV_VK_CHECK_THROW(vkCreateRenderPass2(FVulkanDynamicRHI::GetDevice(), &RenderPassInfo, nullptr, &RenderPass), "failed to create render pass!");

	NumAttachments = NumAttachmentDesc;
}

void FVulkanRenderPass::Release()
{
	for (uint8 i = 0; i < (RTA_MaxColorAttachments + 1) * 2; i++)
	{
		if (FrameImageViews[i])
		{
			vkDestroyImageView(FVulkanDynamicRHI::GetDevice(), FrameImageViews[i], nullptr);
		}
		else
		{
			break;
		}
	}

	if (Framebuffer)
	{
		vkDestroyFramebuffer(FVulkanDynamicRHI::GetDevice(), Framebuffer, nullptr);
	}
	vkDestroyRenderPass(FVulkanDynamicRHI::GetDevice(), RenderPass, nullptr);
}

VkImageView FVulkanRenderPass::CreateImageView(FVulkanTexture* InTexture, int32 ArraySlice, uint8 MipIndex)
{
	REV_CORE_ASSERT(InTexture != nullptr);

	VkImageViewCreateInfo ImageViewCreateInfo{};
	ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ImageViewCreateInfo.image = InTexture->GetImage();
	ImageViewCreateInfo.format = InTexture->GetPlatformFormat();
	ImageViewCreateInfo.subresourceRange.aspectMask = InTexture->GetAspectFlags();

	uint32 TargetLayers = PassDesc.MultiViewCount > 0 ? PassDesc.MultiViewCount : 1;
	uint32 RmainedLayers = ArraySlice < 0 ? InTexture->GetArraySize() : InTexture->GetArraySize() - ArraySlice;

	switch (InTexture->GetDesc().Dimension)
	{
	case ETextureDimension::Texture2D:
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = MipIndex;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;
		break;
	case ETextureDimension::Texture2DArray:
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = MipIndex;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = ArraySlice < 0 ? 0 : (uint32)ArraySlice;
		ImageViewCreateInfo.subresourceRange.layerCount = ArraySlice < 0 ? InTexture->GetArraySize() : Math::Min<uint32>(TargetLayers, RmainedLayers);
		break;
	default:
		REV_CORE_ASSERT(false, "Unsupported render target format.")
		break;
	}

	VkImageView ImageView;
	REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &ImageViewCreateInfo, nullptr, &ImageView));

	return ImageView;
}

uint32 FVulkanRenderPass::GetMultiViewMask(uint8 MultiViewCount)
{
	if(MultiViewCount == 0)
		return 0;

	uint32 MultiViewMask = 0;
	for (uint8 i = 0; i < MultiViewCount; i++)
	{
		MultiViewMask &= 1u << i;
	}
	return MultiViewMask;
}

//DynamicRHI
Ref<FRHIRenderPass> FVulkanDynamicRHI::RHICreateRenderPass(const FRHIRenderPassDesc& InDesc)
{
	return CreateRef<FVulkanRenderPass>(InDesc);
}

}
