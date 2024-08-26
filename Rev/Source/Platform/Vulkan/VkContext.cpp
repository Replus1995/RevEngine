#include "VkContext.h"

#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

//#include <VkBootstrap.h>
#include "VkInitializer.h"
#include "Utils/Image.h"

namespace Rev
{

static VkInstance sInstance = VK_NULL_HANDLE;
static VkDevice sDevice = VK_NULL_HANDLE;
static VmaAllocator sAllocator = VMA_NULL;

FVkContext::FVkContext()
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

FVkContext::~FVkContext()
{
}

void FVkContext::Init()
{
	REV_CORE_ASSERT(sInstance == VK_NULL_HANDLE);
	REV_CORE_ASSERT(sDevice == VK_NULL_HANDLE);
	REV_CORE_ASSERT(sAllocator == VMA_NULL);

	mInstance.CreateInstance();
	mInstance.CreateSurface();
	mDevice.PickPhysicalDevice(&mInstance);
	mDevice.CreateLogicalDevice(&mInstance);
	CreateAllocator();
	
	sInstance = mInstance.GetInstance();
	sDevice = mDevice.GetLogicalDevice();
	sAllocator = mAllocator;

	mSwapchain.CreateSwapchain(&mInstance, &mDevice, mAllocator);
	InitFrameData(mFrameData, REV_VK_FRAME_OVERLAP, &mDevice);

	CreateImmediateData();
}

void FVkContext::Cleanup()
{
	vkDeviceWaitIdle(sDevice);
	
	mMainDeletorQueue.Flush();

	CleanupFrameData(mFrameData, REV_VK_FRAME_OVERLAP, &mDevice);
	mSwapchain.Cleanup(&mDevice, mAllocator);

	sAllocator = VMA_NULL;
	sDevice = VK_NULL_HANDLE;
	sInstance = VK_NULL_HANDLE;

	vmaDestroyAllocator(mAllocator);
	mDevice.Cleanup();
	mInstance.Cleanup();
}

void FVkContext::BeginFrame(bool bClearBackBuffer)
{
	constexpr uint64 kWaitTime = 1000000000;
	auto& CurFrameData = GetFrameData();
	REV_VK_CHECK(vkWaitForFences(sDevice, 1, &CurFrameData.Fence, true, kWaitTime));
	CurFrameData.DeletorQueue.Flush();
	REV_VK_CHECK(vkResetFences(sDevice, 1, &CurFrameData.Fence));

	REV_VK_CHECK(vkAcquireNextImageKHR(sDevice, mSwapchain.GetSwapchain(), kWaitTime, CurFrameData.SwapchainSemaphore, nullptr, &mCurSwapchainImageIndex));

	mDrawExtent.width = mSwapchain.GetBackImage().Extent.width;
	mDrawExtent.height = mSwapchain.GetBackImage().Extent.height;

	VkCommandBuffer CmdBuffer = CurFrameData.MainCmdBuffer;
	REV_VK_CHECK(vkResetCommandBuffer(CmdBuffer, 0));
	VkCommandBufferBeginInfo CmdBufferBeginInfo = FVkInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(CmdBuffer, &CmdBufferBeginInfo));

	if (bClearBackBuffer)
	{
		VkUtils::TransitionImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		ClearBackBuffer();
		VkUtils::TransitionImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
	else
	{
		VkUtils::TransitionImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
	
}

void FVkContext::EndFrame()
{
	//end cmd buffer
	auto& CurFrameData = GetFrameData();
	VkCommandBuffer CmdBuffer = CurFrameData.MainCmdBuffer;


	VkUtils::TransitionImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	VkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	VkUtils::BlitImage(CmdBuffer, mSwapchain.GetBackImage().Image, mSwapchain.GetImages()[mCurSwapchainImageIndex], mDrawExtent, mSwapchain.GetExtent());
	VkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	
}

void FVkContext::PresentFrame()
{
	auto& CurFrameData = GetFrameData();
	VkCommandBuffer CmdBuffer = CurFrameData.MainCmdBuffer;

	VkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	REV_VK_CHECK(vkEndCommandBuffer(CmdBuffer));

	//submit
	VkCommandBufferSubmitInfo CmdBufferInfo = FVkInit::CmdBufferSubmitInfo(CmdBuffer);
	VkSemaphoreSubmitInfo WaitSemaphoreInfo = FVkInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, CurFrameData.SwapchainSemaphore);
	VkSemaphoreSubmitInfo SignalSemaphoreInfo = FVkInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, CurFrameData.RenderSemaphore);

	VkSubmitInfo2 SubmitInfo = FVkInit::SubmitInfo(&CmdBufferInfo, &SignalSemaphoreInfo, &WaitSemaphoreInfo);

	//submit command buffer to the queue and execute it.
	//Fence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(mDevice.GetGraphicsQueue(), 1, &SubmitInfo, CurFrameData.Fence));

	//present
	VkPresentInfoKHR PresentInfo{};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = nullptr;
	PresentInfo.pSwapchains = &mSwapchain.GetSwapchain();
	PresentInfo.swapchainCount = 1;
	PresentInfo.pWaitSemaphores = &CurFrameData.RenderSemaphore;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pImageIndices = &mCurSwapchainImageIndex;
	auto PresentRes = vkQueuePresentKHR(mDevice.GetGraphicsQueue(), &PresentInfo);

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

void FVkContext::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func)
{
	REV_VK_CHECK(vkResetFences(mDevice.GetLogicalDevice(), 1, &mImmFence));
	REV_VK_CHECK(vkResetCommandBuffer(mImmCmdBuffer, 0));

	VkCommandBufferBeginInfo BeginInfo = FVkInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(mImmCmdBuffer, &BeginInfo));

	Func(mImmCmdBuffer);

	REV_VK_CHECK(vkEndCommandBuffer(mImmCmdBuffer));
	VkCommandBufferSubmitInfo CmdBufferSubmitInfo = FVkInit::CmdBufferSubmitInfo(mImmCmdBuffer);
	VkSubmitInfo2 SubmitInfo = FVkInit::SubmitInfo(&CmdBufferSubmitInfo, nullptr, nullptr);

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(mDevice.GetGraphicsQueue(), 1, &SubmitInfo, mImmFence));
	REV_VK_CHECK(vkWaitForFences(mDevice.GetLogicalDevice(), 1, &mImmFence, true, 9999999999));
}

void FVkContext::SetViewport(uint32 InX, uint32 InY, uint32 InWidth, uint32 InHeight)
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

void FVkContext::SetClearColor(const Math::FLinearColor& InColor)
{
	for (size_t i = 0; i < 4; i++)
	{
		mClearColor.float32[i] = InColor[i];
	}
}

void FVkContext::SetClearDepthStencil(float InDepth, uint32 InStencil)
{
	mClearDepthStencil.depth = InDepth;
	mClearDepthStencil.stencil = InStencil;
}

void FVkContext::ClearBackBuffer()
{
	VkImageSubresourceRange ColorImageRange = FVkInit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
	vkCmdClearColorImage(GetMainCmdBuffer(), mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_GENERAL, &mClearColor, 1, &ColorImageRange);
	/*VkImageSubresourceRange DepthImageRange = FVkInit::ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	vkCmdClearDepthStencilImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &mClearDepthStencil, 1, &DepthImageRange);*/
}


void FVkContext::CreateAllocator()
{
	VmaAllocatorCreateInfo AllocatorCreateInfo = {};
	AllocatorCreateInfo.physicalDevice = mDevice.GetPhysicalDevice();
	AllocatorCreateInfo.device = mDevice.GetLogicalDevice();
	AllocatorCreateInfo.instance = mInstance.GetInstance();
	AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&AllocatorCreateInfo, &mAllocator);
}

void FVkContext::CreateImmediateData()
{
	VkCommandPoolCreateInfo CmdPoolCreateInfo = FVkInit::CmdPoolCreateInfo(mDevice.GetQueueFamilyIndices().GraphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	REV_VK_CHECK(vkCreateCommandPool(mDevice.GetLogicalDevice(), &CmdPoolCreateInfo, nullptr, &mImmCmdPool));

	// allocate the command buffer for immediate submits
	VkCommandBufferAllocateInfo cmdAllocInfo = FVkInit::CmdBufferAllocateInfo(mImmCmdPool, 1);
	REV_VK_CHECK(vkAllocateCommandBuffers(mDevice.GetLogicalDevice(), &cmdAllocInfo, &mImmCmdBuffer));

	mMainDeletorQueue.Add(FDeletor{ [=]() {
		vkDestroyCommandPool(mDevice.GetLogicalDevice(), mImmCmdPool, nullptr);
	} });

	VkFenceCreateInfo FenceCreateInfo = FVkInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	REV_VK_CHECK(vkCreateFence(mDevice.GetLogicalDevice(), &FenceCreateInfo, nullptr, &mImmFence));
	mMainDeletorQueue.Add(FDeletor{ [=]() { 
		vkDestroyFence(mDevice.GetLogicalDevice(), mImmFence, nullptr); 
	} });
}


VkInstance FVkCore::GetInstance()
{
	return sInstance;
}

VkDevice FVkCore::GetDevice()
{
	return sDevice;
}

VmaAllocator FVkCore::GetAllocator()
{
	return sAllocator;
}


}