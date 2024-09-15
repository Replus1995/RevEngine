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

	std::vector<VkAttachmentDescription2> AttachmentDescs(mDesc.AttachmentCount);
	for (size_t i = 0; i < mDesc.AttachmentCount; i++)
	{
		const FRenderPassAttachmentDesc& InAttachmentDesc = mDesc.Attachments[i];
		EPixelFormat InFormat = InAttachmentDesc.Format;

		VkAttachmentDescription2 AttachmentDesc{};
		AttachmentDesc.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		AttachmentDesc.format = FVkPixelFormat::TranslatePixelFormat(InFormat, false).Format;
		AttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		AttachmentDesc.loadOp = FVulkanEnum::Translate(InAttachmentDesc.LoadOp);
		AttachmentDesc.storeOp = FVulkanEnum::Translate(InAttachmentDesc.StoreOp);
		AttachmentDesc.stencilLoadOp = FPixelFormatUtils::HasStencil(InFormat) ? FVulkanEnum::Translate(InAttachmentDesc.StencilLoadOp) : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		AttachmentDesc.stencilStoreOp = FPixelFormatUtils::HasStencil(InFormat) ? FVulkanEnum::Translate(InAttachmentDesc.StencilStoreOp) : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		AttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		AttachmentDescs.emplace_back(std::move(AttachmentDesc));
	}

	std::vector<VkSubpassDescription2> SubpassDescs(mDesc.SubPasses.size());
	for (size_t i = 0; i < mDesc.SubPasses.size(); i++)
	{
		const FSubPassDesc& InSubpassDesc = mDesc.SubPasses[i];

		VkSubpassDescription2 SubpassDesc;
		SubpassDesc.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
		SubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDesc.colorAttachmentCount = 1;
		SubpassDesc.pColorAttachments = &colorAttachmentRef;
	}


	VkRenderPassCreateInfo2 RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassInfo.attachmentCount = (uint32_t)AttachmentDescs.size();
	RenderPassInfo.pAttachments = AttachmentDescs.data();
	RenderPassInfo.subpassCount = (uint32_t)SubpassDescs.size();
	RenderPassInfo.pSubpasses = SubpassDescs.data();

	REV_VK_CHECK_THROW(vkCreateRenderPass2(FVulkanCore::GetDevice(), &RenderPassInfo, nullptr, &mRenderPass), "failed to create render pass!");

}


}
