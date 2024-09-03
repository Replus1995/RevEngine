#include "VkContext.h"

#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

//#include <VkBootstrap.h>
#include "VkInitializer.h"
#include "VkUtils.h"

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
	mInstance.CreateInstance();
	mInstance.CreateSurface();
	mDevice.PickPhysicalDevice(&mInstance);
	mDevice.CreateLogicalDevice(&mInstance);
	CreateAllocator();

	mSwapchain.CreateSwapchain(&mInstance, &mDevice, mAllocator);
	InitFrameData(mFrameData, REV_VK_FRAME_OVERLAP, &mDevice);

	CreateImmediateData();
}

void FVulkanContext::Cleanup()
{
	vkDeviceWaitIdle(mDevice.GetLogicalDevice());
	
	mMainDeletorQueue.Flush();

	CleanupFrameData(mFrameData, REV_VK_FRAME_OVERLAP, &mDevice);
	mSwapchain.Cleanup(&mDevice, mAllocator);

	vmaDestroyAllocator(mAllocator);
	mDevice.Cleanup();
	mInstance.Cleanup();
}

void FVulkanContext::Flush()
{
	vkDeviceWaitIdle(mDevice.GetLogicalDevice());
}

void FVulkanContext::BeginFrame(bool bClearBackBuffer)
{
	constexpr uint64 kWaitTime = 1000000000;
	auto& FrameData = GetFrameData();
	REV_VK_CHECK(vkWaitForFences(mDevice.GetLogicalDevice(), 1, &FrameData.Fence, true, kWaitTime));
	FrameData.DeletorQueue.Flush();
	REV_VK_CHECK(vkResetFences(mDevice.GetLogicalDevice(), 1, &FrameData.Fence));

	REV_VK_CHECK(vkAcquireNextImageKHR(mDevice.GetLogicalDevice(), mSwapchain.GetSwapchain(), kWaitTime, FrameData.SwapchainSemaphore, nullptr, &mCurSwapchainImageIndex));

	mDrawExtent.width = mSwapchain.GetBackImage().Extent.width;
	mDrawExtent.height = mSwapchain.GetBackImage().Extent.height;

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
	REV_VK_CHECK(vkQueueSubmit2(mDevice.GetQueue(VQK_Graphics), 1, &SubmitInfo, FrameData.Fence));
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
	auto PresentRes = vkQueuePresentKHR(mDevice.GetQueue(VQK_Graphics), &PresentInfo);

	if (PresentRes == VK_ERROR_OUT_OF_DATE_KHR || PresentRes == VK_SUBOPTIMAL_KHR)
	{
		vkDeviceWaitIdle(mDevice.GetLogicalDevice());
		mSwapchain.Cleanup(&mDevice, mAllocator);
		mSwapchain.CreateSwapchain(&mInstance, &mDevice, mAllocator);
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
	REV_VK_CHECK(vkResetFences(mDevice.GetLogicalDevice(), 1, &mImmFence));
	REV_VK_CHECK(vkResetCommandBuffer(mImmCmdBuffer, 0));

	VkCommandBufferBeginInfo BeginInfo = FVulkanInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(mImmCmdBuffer, &BeginInfo));

	Func(mImmCmdBuffer);

	REV_VK_CHECK(vkEndCommandBuffer(mImmCmdBuffer));
	VkCommandBufferSubmitInfo CmdBufferSubmitInfo = FVulkanInit::CmdBufferSubmitInfo(mImmCmdBuffer);
	VkSubmitInfo2 SubmitInfo = FVulkanInit::SubmitInfo(&CmdBufferSubmitInfo, nullptr, nullptr);

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(mDevice.GetQueue(VQK_Graphics), 1, &SubmitInfo, mImmFence));
	REV_VK_CHECK(vkWaitForFences(mDevice.GetLogicalDevice(), 1, &mImmFence, true, 9999999999));
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


void FVulkanContext::CreateAllocator()
{
	VmaAllocatorCreateInfo AllocatorCreateInfo = {};
	AllocatorCreateInfo.physicalDevice = mDevice.GetPhysicalDevice();
	AllocatorCreateInfo.device = mDevice.GetLogicalDevice();
	AllocatorCreateInfo.instance = mInstance.GetInstance();
	AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&AllocatorCreateInfo, &mAllocator);
}

void FVulkanContext::CreateImmediateData()
{
	VkCommandPoolCreateInfo CmdPoolCreateInfo = FVulkanInit::CmdPoolCreateInfo(mDevice.GetQueueFamily(VQK_Graphics), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	REV_VK_CHECK(vkCreateCommandPool(mDevice.GetLogicalDevice(), &CmdPoolCreateInfo, nullptr, &mImmCmdPool));

	// allocate the command buffer for immediate submits
	VkCommandBufferAllocateInfo cmdAllocInfo = FVulkanInit::CmdBufferAllocateInfo(mImmCmdPool, 1);
	REV_VK_CHECK(vkAllocateCommandBuffers(mDevice.GetLogicalDevice(), &cmdAllocInfo, &mImmCmdBuffer));

	mMainDeletorQueue.Add(FDeletor{ [=]() {
		vkDestroyCommandPool(mDevice.GetLogicalDevice(), mImmCmdPool, nullptr);
	} });

	VkFenceCreateInfo FenceCreateInfo = FVulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	REV_VK_CHECK(vkCreateFence(mDevice.GetLogicalDevice(), &FenceCreateInfo, nullptr, &mImmFence));
	mMainDeletorQueue.Add(FDeletor{ [=]() { 
		vkDestroyFence(mDevice.GetLogicalDevice(), mImmFence, nullptr); 
	} });
}

}