#include "VulkanRenderPass.h"
#include "VulkanPixelFormat.h"
#include "VulkanCore.h"
#include "Core/VulkanEnum.h"
#include "Core/VulkanDefines.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanRenderPass::FVulkanRenderPass(const FRenderPassDesc& InDesc)
	: FRHIRenderPass(InDesc)
{
	REV_CORE_ASSERT(mDesc.AttachmentCount <= (uint32)RTA_MaxColorAttachments);
	CreateResource();
}

FVulkanRenderPass::~FVulkanRenderPass()
{
	ReleaseResource();
}

void FVulkanRenderPass::CreateResource()
{
	uint32 nDepthStencilAttachmentIndex = 0;
	std::vector<VkAttachmentDescription2> AttachmentDescs(mDesc.AttachmentCount);
	for (uint32 i = 0; i < mDesc.AttachmentCount; i++)
	{
		const FRenderPassAttachmentDesc& InAttachmentDesc = mDesc.Attachments[i];
		EPixelFormat InFormat = InAttachmentDesc.Format;

		VkAttachmentDescription2 AttachmentDesc{};
		AttachmentDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		AttachmentDesc.format = (VkFormat)GPixelFormats[InFormat].PlatformFormat;
		AttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDesc.loadOp = FVulkanEnum::Translate(InAttachmentDesc.LoadOp);
		AttachmentDesc.storeOp = FVulkanEnum::Translate(InAttachmentDesc.StoreOp);
		AttachmentDesc.stencilLoadOp = FPixelFormatInfo::HasStencil(InFormat) ? FVulkanEnum::Translate(InAttachmentDesc.StencilLoadOp) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDesc.stencilStoreOp = FPixelFormatInfo::HasStencil(InFormat) ? FVulkanEnum::Translate(InAttachmentDesc.StencilStoreOp) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		AttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		if (FPixelFormatInfo::HasDepth(InFormat))
			nDepthStencilAttachmentIndex = i;

		AttachmentDescs.emplace_back(std::move(AttachmentDesc));
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
		const FSubpassDesc& InSubpassDesc = mDesc.SubpassDescs[i];
		FSubpassAttachmentRef& AttachmentRef = SubpassAttachmentRefs[i];
		VkSubpassDescription2& SubpassDesc = SubpassDescs[i];

		REV_CORE_ASSERT(AttachmentRef.ColorAttachmentCount > 0);

		AttachmentRef.InputAttachmentCount = InSubpassDesc.InputAttachmentCount;
		for (size_t ii = 0; ii < InSubpassDesc.InputAttachmentCount; ii++)
		{
			VkAttachmentReference2& Ref = AttachmentRef.InputAttachments[ii];
			Ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			Ref.pNext = nullptr;
			Ref.attachment = (uint32_t)InSubpassDesc.InputAttachments[ii];
			Ref.layout = InSubpassDesc.InputAttachments[ii] < RTA_MaxColorAttachments ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			Ref.aspectMask = InSubpassDesc.InputAttachments[ii] < RTA_MaxColorAttachments ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		bool bEnableResolve = false;
		AttachmentRef.ColorAttachmentCount = InSubpassDesc.ColorAttachmentCount;
		for (size_t ii = 0; ii < InSubpassDesc.ColorAttachmentCount; ii++)
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
		if (InSubpassDesc.DepthStencilAttachment != RTA_EmptyAttachment)
		{
			VkAttachmentReference2& Ref = AttachmentRef.DepthStencilAttachment;
			Ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			Ref.pNext = nullptr;
			Ref.attachment = nDepthStencilAttachmentIndex;
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
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassInfo.attachmentCount = (uint32_t)AttachmentDescs.size();
	RenderPassInfo.pAttachments = AttachmentDescs.data();
	RenderPassInfo.subpassCount = (uint32_t)SubpassDescs.size();
	RenderPassInfo.pSubpasses = SubpassDescs.data();

	REV_VK_CHECK_THROW(vkCreateRenderPass2(FVulkanCore::GetDevice(), &RenderPassInfo, nullptr, &mRenderPass), "failed to create render pass!");
}

void FVulkanRenderPass::ReleaseResource()
{
	vkDestroyRenderPass(FVulkanCore::GetDevice(), mRenderPass, nullptr);
}


}
