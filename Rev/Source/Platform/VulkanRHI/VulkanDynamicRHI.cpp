#include "VulkanDynamicRHI.h"
#include "VulkanPixelFormat.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include "Rev/Core/Hash.h"

namespace Rev
{

FVulkanDynamicRHI::FVulkanDynamicRHI()
{
}

FVulkanDynamicRHI::~FVulkanDynamicRHI()
{
}

void FVulkanDynamicRHI::Init()
{
	FVulkanPixelFormat::InitPlatformFormats();

	mInstance.CreateInstance();
	mInstance.CreateSurface();
	mInstance.PickPhysicalDevice();
	mInstance.CreateLogicalDevice();

	VmaAllocatorCreateInfo AllocatorCreateInfo = {};
	AllocatorCreateInfo.physicalDevice = mInstance.GetPhysicalDevice();
	AllocatorCreateInfo.device = mInstance.GetDevice();
	AllocatorCreateInfo.instance = mInstance.GetInstance();
	AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&AllocatorCreateInfo, &mAllocator);
}

void FVulkanDynamicRHI::Cleanup()
{
	mSamplerMap.clear();
	vmaDestroyAllocator(mAllocator);
	mInstance.Cleanup();
}

Scope<FRHIContext> FVulkanDynamicRHI::RHICreateContext()
{
	return CreateScope<FVulkanContext>();
}

FVulkanDynamicRHI* FVulkanDynamicRHI::GetVulkanRHI()
{
	return static_cast<FVulkanDynamicRHI*>(GDynamicRHI);
}

}


