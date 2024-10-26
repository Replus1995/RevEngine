#include "VulkanDynamicRHI.h"
#include "VulkanPixelFormat.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanRenderTarget.h"
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

Ref<FRHIUniformBuffer> FVulkanDynamicRHI::CreateUniformBuffer(uint32 InSize)
{
	return CreateRef<FVulkanUniformBuffer>(InSize);
}

Ref<FRHIBuffer> FVulkanDynamicRHI::RHICreateBuffer(uint32 InSize, uint32 InStride, EBufferUsageFlags InFlags)
{
	return CreateRef<FVulkanBuffer>(InSize, InStride, InFlags);
}

Ref<FRHITexture> FVulkanDynamicRHI::CreateTexture(const FRHITextureDesc& InDesc)
{
	return CreateVulkanTexture(InDesc);
}

Ref<FRHIRenderTarget> FVulkanDynamicRHI::CreateRenderTarget(const FRenderTargetDesc& InDesc)
{
	return CreateRef<FVulkanRenderTarget>(InDesc);
}

Ref<FRHIRenderPass> FVulkanDynamicRHI::CreateRenderPass(const FRHIRenderPassDesc& InDesc)
{
	return CreateRef<FVulkanRenderPass>(InDesc);
}

FVulkanDynamicRHI* FVulkanDynamicRHI::GetVulkanRHI()
{
	return static_cast<FVulkanDynamicRHI*>(GDynamicRHI);
}

}


