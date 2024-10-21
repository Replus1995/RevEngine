#include "VulkanRenderPass.h"
#include "VulkanPixelFormat.h"
#include "VulkanDynamicRHI.h"
#include "VulkanRenderTarget.h"
#include "Core/VulkanEnum.h"
#include "Core/VulkanDefines.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanRenderPass::FVulkanRenderPass(const FRHIRenderPassDesc& InDesc)
	: FRHIRenderPass(InDesc)
{
	REV_CORE_ASSERT(mDesc.NumColorAttachments <= (uint32)RTA_MaxColorAttachments);
	CreateResource();
}

FVulkanRenderPass::~FVulkanRenderPass()
{
	ReleaseResource();
}

void FVulkanRenderPass::CreateResource()
{
	int32 nDepthStencilAttachmentIndex = -1;
	std::vector<VkAttachmentDescription2> AttachmentDescs;
	for (uint32 i = 0; i < mDesc.NumColorAttachments; i++)
	{
		const FRHIColorAttachmentDesc& InAttachmentDesc = mDesc.ColorAttachments[i];
		EPixelFormat ColorFormat = InAttachmentDesc.Format;

		VkAttachmentDescription2 AttachmentDesc{};
		AttachmentDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		AttachmentDesc.format = (VkFormat)GPixelFormats[ColorFormat].PlatformFormat;
		AttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDesc.loadOp = FVulkanEnum::Translate(InAttachmentDesc.LoadOp);
		AttachmentDesc.storeOp = FVulkanEnum::Translate(InAttachmentDesc.StoreOp);
		AttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		AttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		AttachmentDescs.emplace_back(std::move(AttachmentDesc));
	}

	if (FPixelFormatInfo::HasDepth(mDesc.DepthStencilAttchment.Format))
	{
		nDepthStencilAttachmentIndex = AttachmentDescs.size();

		EPixelFormat DepthFormat = mDesc.DepthStencilAttchment.Format;
		bool bHasStencil = FPixelFormatInfo::HasStencil(DepthFormat);

		VkAttachmentDescription2 AttachmentDesc{};
		AttachmentDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		AttachmentDesc.format = (VkFormat)GPixelFormats[DepthFormat].PlatformFormat;
		AttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDesc.loadOp = FVulkanEnum::Translate(mDesc.DepthStencilAttchment.LoadOp);
		AttachmentDesc.storeOp = FVulkanEnum::Translate(mDesc.DepthStencilAttchment.StoreOp);
		AttachmentDesc.stencilLoadOp = bHasStencil ? FVulkanEnum::Translate(mDesc.DepthStencilAttchment.StencilLoadOp) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDesc.stencilStoreOp = bHasStencil ? FVulkanEnum::Translate(mDesc.DepthStencilAttchment.StencilStoreOp) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		AttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		AttachmentDescs.emplace_back(std::move(AttachmentDesc));
	}
	else
	{
		mDesc.DepthStencilAttchment.Format = PF_Unknown;
	}

	struct FSubpassAttachmentRef
	{
		uint32 InputAttachmentCount;
		VkAttachmentReference2 InputAttachments[RTA_MaxColorAttachments + 2];
		uint32 ColorAttachmentCount;
		VkAttachmentReference2 ColorAttachments[RTA_MaxColorAttachments];
		VkAttachmentReference2 ResolveAttachments[RTA_MaxColorAttachments];
		VkAttachmentReference2 DepthStencilAttachment;
	};

	std::vector<VkSubpassDescription2> SubpassDescs(mDesc.SubpassDescs.size());
	std::vector<FSubpassAttachmentRef> SubpassAttachmentRefs(mDesc.SubpassDescs.size());
	for (size_t i = 0; i < mDesc.SubpassDescs.size(); i++)
	{
		const FRHISubpassDesc& InSubpassDesc = mDesc.SubpassDescs[i];
		FSubpassAttachmentRef& AttachmentRef = SubpassAttachmentRefs[i];
		VkSubpassDescription2& SubpassDesc = SubpassDescs[i];

		REV_CORE_ASSERT(InSubpassDesc.NumColorAttachments > 0);

		AttachmentRef.InputAttachmentCount = InSubpassDesc.NumInputAttachments;
		for (size_t ii = 0; ii < InSubpassDesc.NumInputAttachments; ii++)
		{
			VkAttachmentReference2& Ref = AttachmentRef.InputAttachments[ii];
			Ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			Ref.pNext = nullptr;
			Ref.attachment = (uint32_t)InSubpassDesc.InputAttachments[ii];
			Ref.layout = InSubpassDesc.InputAttachments[ii] < RTA_MaxColorAttachments ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			Ref.aspectMask = InSubpassDesc.InputAttachments[ii] < RTA_MaxColorAttachments ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		bool bEnableResolve = false;
		AttachmentRef.ColorAttachmentCount = InSubpassDesc.NumColorAttachments;
		for (size_t ii = 0; ii < InSubpassDesc.NumColorAttachments; ii++)
		{
			VkAttachmentReference2& Ref = AttachmentRef.ColorAttachments[ii];
			Ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			Ref.pNext = nullptr;
			Ref.attachment = (uint32_t)InSubpassDesc.ColorAttachments[ii];
			Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			Ref.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			/*
			if (InSubpassDesc.ResolveAttachments[ii] != RTA_EmptyAttachment)
			{
				VkAttachmentReference2& RefResolve = AttachmentRef.ResolveAttachments[ii];
				RefResolve.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
				RefResolve.pNext = nullptr;
				RefResolve.attachment = (uint32_t)InSubpassDesc.ResolveAttachments[ii];
				RefResolve.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				RefResolve.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

				bEnableResolve = true;
			}
			*/
		}

		bool bEnableDepthStencil = false;
		if (InSubpassDesc.DepthStencilAttachment != RTA_EmptyAttachment && nDepthStencilAttachmentIndex >= 0)
		{
			VkAttachmentReference2& Ref = AttachmentRef.DepthStencilAttachment;
			Ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			Ref.pNext = nullptr;
			Ref.attachment = (uint32)nDepthStencilAttachmentIndex;
			Ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			Ref.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

			bEnableDepthStencil = true;
		}

		SubpassDesc.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
		SubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDesc.colorAttachmentCount = AttachmentRef.ColorAttachmentCount;
		SubpassDesc.pColorAttachments = AttachmentRef.ColorAttachments;
		SubpassDesc.pResolveAttachments = bEnableResolve ? AttachmentRef.ResolveAttachments : nullptr;
		SubpassDesc.pDepthStencilAttachment = bEnableDepthStencil ? &AttachmentRef.DepthStencilAttachment : nullptr;
		SubpassDesc.inputAttachmentCount = AttachmentRef.InputAttachmentCount;
		SubpassDesc.pInputAttachments = AttachmentRef.InputAttachmentCount > 0 ? AttachmentRef.InputAttachments : nullptr;
	}


	VkRenderPassCreateInfo2 RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
	RenderPassInfo.attachmentCount = (uint32_t)AttachmentDescs.size();
	RenderPassInfo.pAttachments = AttachmentDescs.data();
	RenderPassInfo.subpassCount = (uint32_t)SubpassDescs.size();
	RenderPassInfo.pSubpasses = SubpassDescs.data();

	REV_VK_CHECK_THROW(vkCreateRenderPass2(FVulkanDynamicRHI::GetDevice(), &RenderPassInfo, nullptr, &mRenderPass), "failed to create render pass!");
}

void FVulkanRenderPass::ReleaseResource()
{
	vkDestroyRenderPass(FVulkanDynamicRHI::GetDevice(), mRenderPass, nullptr);
}


}
