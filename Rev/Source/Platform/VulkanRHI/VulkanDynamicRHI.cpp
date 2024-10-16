#include "VulkanDynamicRHI.h"
#include "VulkanPixelFormat.h"
#include "VulkanBuffer.h"
#include "VulkanUniform.h"
#include "VulkanSampler.h"
#include "VulkanTexture.h"
#include "VulkanRenderTarget.h"
#include "VulkanPrimitive.h"
#include "VulkanRenderPass.h"

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
	vmaDestroyAllocator(mAllocator);
	mInstance.Cleanup();
}

Scope<FRHIContext> FVulkanDynamicRHI::CreateContext()
{
	return CreateScope<FVulkanContext>();
}

Ref<FRHIVertexBuffer> FVulkanDynamicRHI::CreateVertexBuffer(uint32 InSize, bool bDynamic)
{
	return CreateRef<FVulkanVertexBuffer>(InSize, bDynamic);
}

Ref<FRHIIndexBuffer> FVulkanDynamicRHI::CreateIndexBuffer(uint32 InStride, uint32 InCount, bool bDynamic)
{
	return CreateRef<FVulkanIndexBuffer>(InStride, InCount, bDynamic);
}

Ref<FRHIUniformBuffer> FVulkanDynamicRHI::CreateUniformBuffer(uint32 InSize)
{
	return CreateRef<FVulkanUniformBuffer>(InSize);
}

Ref<FRHIPrimitive> FVulkanDynamicRHI::CreatePrimitive(EPrimitiveTopology InTopology)
{
	return CreateRef<FVulkanPrimitive>(InTopology);
}

Ref<FRHITexture> FVulkanDynamicRHI::CreateTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
{
	return CreateVulkanTexture(InDesc, InSamplerDesc);
}

Ref<FRHIRenderTarget> FVulkanDynamicRHI::CreateRenderTarget(const FRenderTargetDesc& InDesc)
{
	return CreateRef<FVulkanRenderTarget>(InDesc);
}

Ref<FRHIRenderPass> FVulkanDynamicRHI::CreateRenderPass(const FRenderPassDesc& InDesc)
{
	return CreateRef<FVulkanRenderPass>(InDesc);
}

FVulkanDynamicRHI* FVulkanDynamicRHI::GetVulkanRHI()
{
	return static_cast<FVulkanDynamicRHI*>(GDynamicRHI);
}

}


