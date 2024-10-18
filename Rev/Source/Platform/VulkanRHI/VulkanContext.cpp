#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "VulkanUniform.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanRenderTarget.h"
#include "VulkanPrimitive.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanPipeline.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"

#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

//#include <VkBootstrap.h>

namespace Rev
{
FVulkanContext::FVulkanContext()
{
	for (size_t i = 0; i < 4; i++)
	{
		mClearColor.float32[i] = 0.0f;
		mClearColor.int32[i] = 0;
		mClearColor.uint32[i] = 0;
	}
	mClearDepthStencil.depth = 1.0f;
	mClearDepthStencil.stencil = 0;
}

FVulkanContext::~FVulkanContext()
{
}

void FVulkanContext::Init()
{
	mCurPresentMode = mTargetPresentMode;
	mSwapchain.CreateSwapchain(mCurPresentMode);
	InitFrameData(mFrameData, REV_VK_FRAME_OVERLAP);
	CreateImmediateData();
}

void FVulkanContext::Cleanup()
{
	mGraphicsPipelineCache.ClearAll();

	vkDestroyCommandPool(FVulkanDynamicRHI::GetDevice(), mImmCmdPool, nullptr);
	vkDestroyFence(FVulkanDynamicRHI::GetDevice(), mImmFence, nullptr);

	vkDeviceWaitIdle(FVulkanDynamicRHI::GetDevice());
	CleanupFrameData(mFrameData, REV_VK_FRAME_OVERLAP);
	mSwapchain.Cleanup();
}

void FVulkanContext::Flush()
{
	vkDeviceWaitIdle(FVulkanDynamicRHI::GetDevice());
}

void FVulkanContext::BeginFrame(bool bClearBackBuffer)
{
	constexpr uint64 kWaitTime = 1000000000;
	auto& FrameData = GetActiveFrameData();
	REV_VK_CHECK(vkWaitForFences(FVulkanDynamicRHI::GetDevice(), 1, &FrameData.Fence, true, kWaitTime));
	REV_VK_CHECK(vkResetFences(FVulkanDynamicRHI::GetDevice(), 1, &FrameData.Fence));

	REV_VK_CHECK(vkAcquireNextImageKHR(FVulkanDynamicRHI::GetDevice(), mSwapchain.GetSwapchain(), kWaitTime, FrameData.SwapchainSemaphore, nullptr, &mCurSwapchainImageIndex));

	mDrawExtent.width = mSwapchain.GetExtent().width;
	mDrawExtent.height = mSwapchain.GetExtent().height;

	FrameData.DescriptorPool.ResetPool(FVulkanDynamicRHI::GetDevice());

	VkCommandBuffer CmdBuffer = FrameData.CmdBuffer;
	REV_VK_CHECK(vkResetCommandBuffer(CmdBuffer, 0));
	VkCommandBufferBeginInfo CmdBufferBeginInfo = FVulkanInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(CmdBuffer, &CmdBufferBeginInfo));

	if (bClearBackBuffer)
	{
		FVulkanUtils::TransitionImage(CmdBuffer, GetSwapchainImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		ClearBackBuffer();
		FVulkanUtils::TransitionImage(CmdBuffer, GetSwapchainImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
	else
	{
		FVulkanUtils::TransitionImage(CmdBuffer, GetSwapchainImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}

}

void FVulkanContext::EndFrame()
{
	//end cmd buffer
	auto& FrameData = GetActiveFrameData();
	VkCommandBuffer CmdBuffer = FrameData.CmdBuffer;

	/*FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	FVkUtils::BlitImage(CmdBuffer, mSwapchain.GetBackImage().Image, mSwapchain.GetImages()[mCurSwapchainImageIndex], mDrawExtent, mSwapchain.GetExtent());
	FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);*/

	FVulkanUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	REV_VK_CHECK(vkEndCommandBuffer(CmdBuffer));

	//submit
	VkCommandBufferSubmitInfo CmdBufferInfo = FVulkanInit::CmdBufferSubmitInfo(CmdBuffer);
	VkSemaphoreSubmitInfo WaitSemaphoreInfo = FVulkanInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, FrameData.SwapchainSemaphore);
	VkSemaphoreSubmitInfo SignalSemaphoreInfo = FVulkanInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, FrameData.RenderSemaphore);

	VkSubmitInfo2 SubmitInfo = FVulkanInit::SubmitInfo(&CmdBufferInfo, &SignalSemaphoreInfo, &WaitSemaphoreInfo);

	//submit command buffer to the queue and execute it.
	//Fence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(FVulkanDynamicRHI::GetQueue(VQK_Graphics), 1, &SubmitInfo, FrameData.Fence));

	//clear bindings
	mTextures.clear();
	mUniformBuffers.clear();
	mCurProgram = nullptr;
	mCurRenderPass = nullptr;
}

void FVulkanContext::PresentFrame()
{
	auto& FrameData = GetActiveFrameData();

	//present
	VkPresentInfoKHR PresentInfo{};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = nullptr;
	PresentInfo.pSwapchains = &mSwapchain.GetSwapchain();
	PresentInfo.swapchainCount = 1;
	PresentInfo.pWaitSemaphores = &FrameData.RenderSemaphore;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pImageIndices = &mCurSwapchainImageIndex;
	auto PresentRes = vkQueuePresentKHR(FVulkanDynamicRHI::GetQueue(VQK_Graphics), &PresentInfo);

	if (PresentRes == VK_ERROR_OUT_OF_DATE_KHR || PresentRes == VK_SUBOPTIMAL_KHR || mCurPresentMode != mTargetPresentMode)
	{
		vkDeviceWaitIdle(FVulkanDynamicRHI::GetDevice());
		mSwapchain.Cleanup();
		mCurPresentMode = mTargetPresentMode;
		mSwapchain.CreateSwapchain(mCurPresentMode);
	}
	else if (PresentRes != VK_SUCCESS)
	{
		throw std::runtime_error("[FVkContext] Failed to present swap chain image!");
	}

	mFrameDataIndex = (mFrameDataIndex + 1) % REV_VK_FRAME_OVERLAP;
	//mCurSwapchainImageIndex = 0;

}

void FVulkanContext::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func)
{
	REV_VK_CHECK(vkResetFences(FVulkanDynamicRHI::GetDevice(), 1, &mImmFence));
	REV_VK_CHECK(vkResetCommandBuffer(mImmCmdBuffer, 0));

	VkCommandBufferBeginInfo BeginInfo = FVulkanInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(mImmCmdBuffer, &BeginInfo));

	Func(mImmCmdBuffer);

	REV_VK_CHECK(vkEndCommandBuffer(mImmCmdBuffer));
	VkCommandBufferSubmitInfo CmdBufferSubmitInfo = FVulkanInit::CmdBufferSubmitInfo(mImmCmdBuffer);
	VkSubmitInfo2 SubmitInfo = FVulkanInit::SubmitInfo(&CmdBufferSubmitInfo, nullptr, nullptr);

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(FVulkanDynamicRHI::GetQueue(VQK_Graphics), 1, &SubmitInfo, mImmFence));
	REV_VK_CHECK(vkWaitForFences(FVulkanDynamicRHI::GetDevice(), 1, &mImmFence, true, 9999999999));
}

void FVulkanContext::SetVSync(bool bEnable)
{
	mTargetPresentMode = bEnable ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
}

void FVulkanContext::SetViewport(uint32 InX, uint32 InY, uint32 InWidth, uint32 InHeight)
{
	mViewport.x = InX;
	mViewport.y = InY + InHeight;
	mViewport.width = InWidth;
	mViewport.height = InHeight * -1.0f; //flip viewport to use opengl coordinates
	mViewport.minDepth = 0.0f;
	mViewport.maxDepth = 1.0f;

	mScissor.offset.x = InX;
	mScissor.offset.y = InY;
	mScissor.extent.width = InWidth;
	mScissor.extent.height = InHeight;
}

void FVulkanContext::SetClearColor(const Math::FLinearColor& InColor)
{
	for (size_t i = 0; i < 4; i++)
	{
		mClearColor.float32[i] = InColor[i];
	}
}

void FVulkanContext::SetClearDepthStencil(float InDepth, uint32 InStencil)
{
	mClearDepthStencil.depth = InDepth;
	mClearDepthStencil.stencil = InStencil;
}

void FVulkanContext::ClearBackBuffer()
{
	VkImageSubresourceRange ColorImageRange = FVulkanInit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
	vkCmdClearColorImage(GetActiveCmdBuffer(), GetSwapchainImage(), VK_IMAGE_LAYOUT_GENERAL, &mClearColor, 1, &ColorImageRange);
	/*VkImageSubresourceRange DepthImageRange = FVkInit::ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	vkCmdClearDepthStencilImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &mClearDepthStencil, 1, &DepthImageRange);*/
}

void FVulkanContext::UpdateTexture(FRHITexture* InTexture, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
	if(!InTexture) return;
	FVulkanTexture::Cast(InTexture)->UpdateContent(this, InContent, InSize, InMipLevel, InArrayIndex);
}

void FVulkanContext::ClearTexture(FRHITexture* InTexture, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount)
{
	if (!InTexture) return;
	FVulkanTexture::Cast(InTexture)->ClearContent(this, InMipLevel, InMipCount, InArrayIndex, InArrayCount);
}

void FVulkanContext::UpdateBufferData(const Ref<FRHIVertexBuffer>& Buffer, const void* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= Buffer->GetCapacity());
	FVulkanUtils::ImmediateUploadBuffer(this, (VkBuffer)Buffer->GetNativeHandle(), Content, Size, Offset);
}

void FVulkanContext::UpdateBufferData(const Ref<FRHIIndexBuffer>& Buffer, const void* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= Buffer->GetCapacity());
	FVulkanUtils::ImmediateUploadBuffer(this, (VkBuffer)Buffer->GetNativeHandle(), Content, Size, Offset);
}

void FVulkanContext::BeginRenderPass(const Ref<FRHIRenderPass>& InRenderPass)
{
	mCurRenderPass = std::static_pointer_cast<FVulkanRenderPass>(InRenderPass);
	if(!mCurRenderPass)
		return;
	if (!mCurRenderPass->GetRenderTarget())
	{
		mCurRenderPass = nullptr;
		return;
	}
	FVulkanRenderTarget* pRenderTarget = static_cast<FVulkanRenderTarget*>(mCurRenderPass->GetRenderTarget().get());
	pRenderTarget->FlushResource((VkRenderPass)mCurRenderPass->GetNativeHandle());

	VkRenderPassBeginInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassInfo.pNext = NULL;
	RenderPassInfo.renderPass = (VkRenderPass)mCurRenderPass->GetNativeHandle();
	RenderPassInfo.framebuffer = (VkFramebuffer)pRenderTarget->GetNativeHandle();
	RenderPassInfo.renderArea.offset.x = 0;
	RenderPassInfo.renderArea.offset.y = 0;
	RenderPassInfo.renderArea.extent.width = mCurRenderPass->GetRenderTarget()->GetWidth();
	RenderPassInfo.renderArea.extent.height = mCurRenderPass->GetRenderTarget()->GetHeight();
	RenderPassInfo.clearValueCount = pRenderTarget->GetNumClearValues();
	RenderPassInfo.pClearValues = pRenderTarget->GetClearValues();

	VkSubpassBeginInfo SubpassBeginInfo{};
	SubpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
	SubpassBeginInfo.pNext = NULL;
	SubpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;

	vkCmdBeginRenderPass2(GetActiveCmdBuffer(), &RenderPassInfo, &SubpassBeginInfo);

	vkCmdSetViewport(GetActiveCmdBuffer(), 0, 1, &mViewport);
	vkCmdSetScissor(GetActiveCmdBuffer(), 0, 1, &mScissor);
}

void FVulkanContext::EndRenderPass(bool bBlitToBack)
{
	if(!mCurRenderPass)
		return;

	VkSubpassEndInfo SubpassEndInfo{};
	SubpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;
	SubpassEndInfo.pNext = NULL;

	vkCmdEndRenderPass2(GetActiveCmdBuffer(), &SubpassEndInfo);



	if (bBlitToBack)
	{
		auto ColorTex = mCurRenderPass->GetRenderTarget()->GetTargetTexture(RTA_ColorAttachment0);
		FVulkanUtils::TransitionImage(GetActiveCmdBuffer(), (VkImage)ColorTex->GetNativeHandle(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		FVulkanUtils::TransitionImage(GetActiveCmdBuffer(), mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		FVulkanUtils::BlitImage(GetActiveCmdBuffer(), (VkImage)ColorTex->GetNativeHandle(), mSwapchain.GetImages()[mCurSwapchainImageIndex], mDrawExtent, mSwapchain.GetExtent());
		FVulkanUtils::TransitionImage(GetActiveCmdBuffer(), mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
}

void FVulkanContext::NextSubpass()
{
	VkSubpassBeginInfo SubpassBeginInfo{};
	SubpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
	SubpassBeginInfo.pNext = NULL;
	SubpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;

	VkSubpassEndInfo SubpassEndInfo{};
	SubpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;
	SubpassEndInfo.pNext = NULL;

	vkCmdNextSubpass2(GetActiveCmdBuffer(), &SubpassBeginInfo, &SubpassEndInfo);
}

void FVulkanContext::BindUniformBuffer(const Ref<FRHIUniformBuffer>& InBuffer, uint16 InBinding)
{
	if(!InBuffer) return;
	mUniformBuffers[InBinding] = static_cast<FVulkanUniformBuffer*>(InBuffer.get());
}

void FVulkanContext::BindTexture(const Ref<FRHITexture>& InTexture, uint16 InBinding)
{
	if (!InTexture) return;
	mTextures[InBinding] = static_cast<FVulkanTexture*>(InTexture.get());
}

void FVulkanContext::BindProgram(const Ref<FRHIShaderProgram>& InProgram)
{
	mCurProgram = std::static_pointer_cast<FVulkanShaderProgram>(InProgram);
}

void FVulkanContext::SetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState)
{
	mCurState = InState;
}

void FVulkanContext::DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive)
{
	if (!mCurRenderPass || !mCurProgram || !InPrimitive)
	{
		return;
	}

	FVulkanRenderPass* RenderPass = static_cast<FVulkanRenderPass*>(mCurRenderPass.get());
	FVulkanShaderProgram* ShaderProgram = static_cast<FVulkanShaderProgram*>(mCurProgram.get());
	FVulkanPrimitive* Primitive = static_cast<FVulkanPrimitive*>(InPrimitive.get());
	Primitive->PrepareDraw();

	FVulkanPipeline* GraphicsPipeline = mGraphicsPipelineCache.GetOrCreatePipeline(mCurState, RenderPass, ShaderProgram, Primitive);

	if (!GraphicsPipeline || !GraphicsPipeline->PipelineLayout)
		return;

	FVulkanPipelineLayout* GraphicsPipelineLayout = GraphicsPipeline->PipelineLayout;

	VkDescriptorSet DescSet = GetDescriptorSet(ShaderProgram, GraphicsPipelineLayout);
	vkCmdBindDescriptorSets(GetActiveCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipelineLayout->PipelineLayout, 0, 1, &DescSet, 0, nullptr);
	vkCmdBindPipeline(GetActiveCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline->Pipeline);

	VkBuffer VertexBuffers[REV_VK_MAX_VERTEX_STREAMS];
	VkDeviceSize VertexOffsets[REV_VK_MAX_VERTEX_STREAMS];
	uint32 VertexStreamCount = Primitive->GetVertexBuffers().size();

	REV_CORE_ASSERT(VertexStreamCount < REV_VK_MAX_VERTEX_STREAMS);

	for (uint32 i = 0; i < VertexStreamCount; i++)
	{
		const Ref<FRHIVertexBuffer>& VertexBuffer = Primitive->GetVertexBuffers()[i];
		VertexBuffers[i] = (VkBuffer)VertexBuffer->GetNativeHandle();
		VertexOffsets[i] = 0; //temp, to be modifed
	}

	FVulkanIndexBuffer* IndexBuffer = static_cast<FVulkanIndexBuffer*>(Primitive->GetIndexBuffer().get());

	vkCmdBindVertexBuffers2(GetActiveCmdBuffer(), 0, VertexStreamCount, VertexBuffers, VertexOffsets, NULL, NULL);
	vkCmdBindIndexBuffer(GetActiveCmdBuffer(), (VkBuffer)Primitive->GetIndexBuffer()->GetNativeHandle(), 0, IndexBuffer->GetIndexType());
	vkCmdDrawIndexed(GetActiveCmdBuffer(), Primitive->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
}

FVulkanUniformBuffer* FVulkanContext::FindUniformBuffer(uint16 BindingIdx) const
{
	if (auto iter = mUniformBuffers.find(BindingIdx); iter != mUniformBuffers.end())
	{
		return iter->second;
	}
	return nullptr;
}

FVulkanContext* FVulkanContext::Cast(FRHIContext* InContext)
{
	REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);
	return static_cast<FVulkanContext*>(InContext);
}

void FVulkanContext::CreateImmediateData()
{
	VkCommandPoolCreateInfo CmdPoolCreateInfo = FVulkanInit::CmdPoolCreateInfo(FVulkanDynamicRHI::GetQueueFamily(VQK_Graphics), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	REV_VK_CHECK(vkCreateCommandPool(FVulkanDynamicRHI::GetDevice(), &CmdPoolCreateInfo, nullptr, &mImmCmdPool));

	// allocate the command buffer for immediate submits
	VkCommandBufferAllocateInfo cmdAllocInfo = FVulkanInit::CmdBufferAllocateInfo(mImmCmdPool, 1);
	REV_VK_CHECK(vkAllocateCommandBuffers(FVulkanDynamicRHI::GetDevice(), &cmdAllocInfo, &mImmCmdBuffer));

	VkFenceCreateInfo FenceCreateInfo = FVulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	REV_VK_CHECK(vkCreateFence(FVulkanDynamicRHI::GetDevice(), &FenceCreateInfo, nullptr, &mImmFence));
}

VkDescriptorSet FVulkanContext::GetDescriptorSet(const FVulkanShaderProgram* InProgram, const FVulkanPipelineLayout* InLayout)
{
	VkDescriptorSetLayout DescLayout = InLayout->DescriptorSetLayout;

	VkDescriptorSet DescSet;
	VkDescriptorSetAllocateInfo DescAllocateInfo{};
	DescAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescAllocateInfo.pNext = NULL;
	DescAllocateInfo.descriptorPool = GetActiveDescriptorPool().Pool;
	DescAllocateInfo.descriptorSetCount = 1;
	DescAllocateInfo.pSetLayouts = &DescLayout;

	REV_VK_CHECK(vkAllocateDescriptorSets(FVulkanDynamicRHI::GetDevice(), &DescAllocateInfo, &DescSet));


	VkDescriptorBufferInfo BufferInfos[REV_VK_MAX_SHADER_UNIFORM_BUFFERS] = {};
	VkWriteDescriptorSet Writes[REV_VK_MAX_DESCRIPTORSETS] = {};
	uint32 WriteCount = 0;
	uint32_t BufferCount = 0;

	for (const FRHIUniformInfo& Uniform : InProgram->GetProgramUniforms())
	{
		uint32 BindingIdx = Uniform.Binding;
		FVulkanUniformBuffer* UniformBuffer = FindUniformBuffer(BindingIdx);
		if (!UniformBuffer)
			continue;

		BufferInfos[BufferCount].buffer = (VkBuffer)UniformBuffer->GetNativeHandle();
		BufferInfos[BufferCount].offset = 0;
		BufferInfos[BufferCount].range = UniformBuffer->GetSize();

		Writes[WriteCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Writes[WriteCount].pNext = NULL;
		Writes[WriteCount].dstSet = DescSet;
		Writes[WriteCount].dstBinding = BindingIdx;
		Writes[WriteCount].dstArrayElement = 0;
		Writes[WriteCount].descriptorCount = 1;
		Writes[WriteCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		Writes[WriteCount].pImageInfo = NULL;
		Writes[WriteCount].pBufferInfo = &BufferInfos[BufferCount];
		Writes[WriteCount].pTexelBufferView = NULL;

		++WriteCount;
		++BufferCount;
	}

	vkUpdateDescriptorSets(FVulkanDynamicRHI::GetDevice(), WriteCount, Writes, 0, NULL);

	return DescSet;
}

}