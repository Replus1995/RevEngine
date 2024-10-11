#include "VulkanContext.h"
#include "VulkanCore.h"
#include "VulkanUtils.h"
#include "VulkanUniform.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanRenderTarget.h"
#include "VulkanPrimitive.h"
#include "VulkanRenderPass.h"
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
	vkDestroyCommandPool(FVulkanCore::GetDevice(), mImmCmdPool, nullptr);
	vkDestroyFence(FVulkanCore::GetDevice(), mImmFence, nullptr);

	vkDeviceWaitIdle(FVulkanCore::GetDevice());
	CleanupFrameData(mFrameData, REV_VK_FRAME_OVERLAP);
	mSwapchain.Cleanup();
}

void FVulkanContext::Flush()
{
	vkDeviceWaitIdle(FVulkanCore::GetDevice());
}

void FVulkanContext::BeginFrame(bool bClearBackBuffer)
{
	constexpr uint64 kWaitTime = 1000000000;
	auto& FrameData = GetFrameData();
	REV_VK_CHECK(vkWaitForFences(FVulkanCore::GetDevice(), 1, &FrameData.Fence, true, kWaitTime));
	REV_VK_CHECK(vkResetFences(FVulkanCore::GetDevice(), 1, &FrameData.Fence));

	REV_VK_CHECK(vkAcquireNextImageKHR(FVulkanCore::GetDevice(), mSwapchain.GetSwapchain(), kWaitTime, FrameData.SwapchainSemaphore, nullptr, &mCurSwapchainImageIndex));

	mDrawExtent.width = mSwapchain.GetExtent().width;
	mDrawExtent.height = mSwapchain.GetExtent().height;

	FrameData.DescriptorPool.ResetPool(FVulkanCore::GetDevice());

	VkCommandBuffer CmdBuffer = FrameData.MainCmdBuffer;
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
	auto& FrameData = GetFrameData();
	VkCommandBuffer CmdBuffer = FrameData.MainCmdBuffer;


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
	REV_VK_CHECK(vkQueueSubmit2(FVulkanCore::GetQueue(VQK_Graphics), 1, &SubmitInfo, FrameData.Fence));

	//clear bindings
	mTextures.clear();
	mUniformBuffers.clear();
	mCurProgram = nullptr;
	mCurRenderPass = nullptr;
}

void FVulkanContext::PresentFrame()
{
	auto& FrameData = GetFrameData();

	//present
	VkPresentInfoKHR PresentInfo{};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = nullptr;
	PresentInfo.pSwapchains = &mSwapchain.GetSwapchain();
	PresentInfo.swapchainCount = 1;
	PresentInfo.pWaitSemaphores = &FrameData.RenderSemaphore;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pImageIndices = &mCurSwapchainImageIndex;
	auto PresentRes = vkQueuePresentKHR(FVulkanCore::GetQueue(VQK_Graphics), &PresentInfo);

	if (PresentRes == VK_ERROR_OUT_OF_DATE_KHR || PresentRes == VK_SUBOPTIMAL_KHR || mCurPresentMode != mTargetPresentMode)
	{
		vkDeviceWaitIdle(FVulkanCore::GetDevice());
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
	REV_VK_CHECK(vkResetFences(FVulkanCore::GetDevice(), 1, &mImmFence));
	REV_VK_CHECK(vkResetCommandBuffer(mImmCmdBuffer, 0));

	VkCommandBufferBeginInfo BeginInfo = FVulkanInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(mImmCmdBuffer, &BeginInfo));

	Func(mImmCmdBuffer);

	REV_VK_CHECK(vkEndCommandBuffer(mImmCmdBuffer));
	VkCommandBufferSubmitInfo CmdBufferSubmitInfo = FVulkanInit::CmdBufferSubmitInfo(mImmCmdBuffer);
	VkSubmitInfo2 SubmitInfo = FVulkanInit::SubmitInfo(&CmdBufferSubmitInfo, nullptr, nullptr);

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(FVulkanCore::GetQueue(VQK_Graphics), 1, &SubmitInfo, mImmFence));
	REV_VK_CHECK(vkWaitForFences(FVulkanCore::GetDevice(), 1, &mImmFence, true, 9999999999));
}

void FVulkanContext::SetVSync(bool bEnable)
{
	mTargetPresentMode = bEnable ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
}

void FVulkanContext::SetViewport(uint32 InX, uint32 InY, uint32 InWidth, uint32 InHeight)
{
	VkViewport Viewport{};
	Viewport.x = InX;
	Viewport.y = InY;
	Viewport.width = InWidth;
	Viewport.height = InHeight;
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;
	vkCmdSetViewport(GetMainCmdBuffer(), 0, 1, &Viewport);
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
	vkCmdClearColorImage(GetMainCmdBuffer(), GetSwapchainImage(), VK_IMAGE_LAYOUT_GENERAL, &mClearColor, 1, &ColorImageRange);
	/*VkImageSubresourceRange DepthImageRange = FVkInit::ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	vkCmdClearDepthStencilImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &mClearDepthStencil, 1, &DepthImageRange);*/
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

	VkRenderPassBeginInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassInfo.pNext = NULL;
	RenderPassInfo.renderPass = (VkRenderPass)mCurRenderPass->GetNativeHandle();
	RenderPassInfo.framebuffer = (VkFramebuffer)mCurRenderPass->GetRenderTarget()->GetNativeHandle();
	RenderPassInfo.renderArea.offset.x = 0;
	RenderPassInfo.renderArea.offset.y = 0;
	RenderPassInfo.renderArea.extent.width = mCurRenderPass->GetRenderTarget()->GetWidth();
	RenderPassInfo.renderArea.extent.height = mCurRenderPass->GetRenderTarget()->GetHeight();
	RenderPassInfo.clearValueCount = 0;
	RenderPassInfo.pClearValues = NULL;

	VkSubpassBeginInfo SubpassBeginInfo{};
	SubpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
	SubpassBeginInfo.pNext = NULL;
	SubpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;

	vkCmdBeginRenderPass2(GetMainCmdBuffer(), &RenderPassInfo, &SubpassBeginInfo);
}

void FVulkanContext::EndRenderPass()
{
	VkSubpassEndInfo SubpassEndInfo{};
	SubpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;
	SubpassEndInfo.pNext = NULL;

	vkCmdEndRenderPass2(GetMainCmdBuffer(), &SubpassEndInfo);
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

	vkCmdNextSubpass2(GetMainCmdBuffer(), &SubpassBeginInfo, &SubpassEndInfo);
}

void FVulkanContext::BindUniformBuffer(const Ref<FRHIUniformBuffer>& InBuffer, uint16 InBinding)
{
	mUniformBuffers[InBinding] = std::static_pointer_cast<FVulkanUniformBuffer>(InBuffer);
}

void FVulkanContext::BindTexture(const Ref<FRHITexture>& InTexture, uint16 InBinding)
{
	mTextures[InBinding] = std::static_pointer_cast<FVulkanTexture>(InTexture);
}

void FVulkanContext::BindProgram(const Ref<FRHIShaderProgram>& InProgram)
{
	mCurProgram = std::static_pointer_cast<FVulkanShaderProgram>(InProgram);
}

void FVulkanContext::DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive)
{
	if (!mCurRenderPass || !mCurProgram || !InPrimitive)
	{
		return;
	}

	const FVulkanRenderTarget* RenderTarget = static_cast<FVulkanRenderTarget*>(mCurRenderPass->GetRenderTarget().get());
	const FVulkanPrimitive* Primitive = static_cast<FVulkanPrimitive*>(InPrimitive.get());

	mCurProgram->PrepareDraw(RenderTarget, Primitive);

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

	vkCmdBindVertexBuffers2(GetMainCmdBuffer(), 0, VertexStreamCount, VertexBuffers, VertexOffsets, NULL, NULL);
	vkCmdBindIndexBuffer(GetMainCmdBuffer(), (VkBuffer)Primitive->GetIndexBuffer()->GetNativeHandle(), 0, FVulkanEnum::Translate(Primitive->GetIndexBuffer()->GetType()));
	vkCmdDrawIndexed(GetMainCmdBuffer(), Primitive->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
}

FVulkanUniformBuffer* FVulkanContext::FindUniformBuffer(uint16 BindingIdx) const
{
	if (auto iter = mUniformBuffers.find(BindingIdx); iter != mUniformBuffers.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

void FVulkanContext::CreateImmediateData()
{
	VkCommandPoolCreateInfo CmdPoolCreateInfo = FVulkanInit::CmdPoolCreateInfo(FVulkanCore::GetQueueFamily(VQK_Graphics), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	REV_VK_CHECK(vkCreateCommandPool(FVulkanCore::GetDevice(), &CmdPoolCreateInfo, nullptr, &mImmCmdPool));

	// allocate the command buffer for immediate submits
	VkCommandBufferAllocateInfo cmdAllocInfo = FVulkanInit::CmdBufferAllocateInfo(mImmCmdPool, 1);
	REV_VK_CHECK(vkAllocateCommandBuffers(FVulkanCore::GetDevice(), &cmdAllocInfo, &mImmCmdBuffer));

	VkFenceCreateInfo FenceCreateInfo = FVulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	REV_VK_CHECK(vkCreateFence(FVulkanCore::GetDevice(), &FenceCreateInfo, nullptr, &mImmFence));
}

}