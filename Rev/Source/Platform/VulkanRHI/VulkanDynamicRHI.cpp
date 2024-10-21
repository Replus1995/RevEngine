#include "VulkanDynamicRHI.h"
#include "VulkanPixelFormat.h"
#include "VulkanBuffer.h"
#include "VulkanUniform.h"
#include "VulkanState.h"
#include "VulkanTexture.h"
#include "VulkanRenderTarget.h"
#include "VulkanPrimitive.h"
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

Ref<FRHITexture> FVulkanDynamicRHI::CreateTexture(const FRHITextureDesc& InDesc)
{
	return CreateVulkanTexture(InDesc);
}

Ref<FRHIRenderTarget> FVulkanDynamicRHI::CreateRenderTarget(const FRenderTargetDesc& InDesc)
{
	return CreateRef<FVulkanRenderTarget>(InDesc);
}

Ref<FRHISamplerState> FVulkanDynamicRHI::CreateSamplerState(const FRHISamplerStateDesc& InDesc)
{
	VkSamplerCreateInfo SamplerCreateInfo;
	FVulkanSamplerState::FillCreateInfo(InDesc, SamplerCreateInfo);

	uint32 hash = FCityHash::Gen(&SamplerCreateInfo, sizeof(SamplerCreateInfo));

	//Add lock for multithread create
	{
		if (auto iter = mSamplerMap.find(hash); iter != mSamplerMap.end())
		{
			return iter->second;
		}
		Ref<FVulkanSamplerState> NewSampler = CreateRef<FVulkanSamplerState>(SamplerCreateInfo);
		mSamplerMap.emplace(hash, NewSampler);
		return NewSampler;
	}
}

Ref<FRHIRasterizerState> FVulkanDynamicRHI::CreateRasterizerState(const FRHIRasterizerStateDesc& InDesc)
{
	return CreateRef<FVulkanRasterizerState>(InDesc);
}

Ref<FRHIDepthStencilState> FVulkanDynamicRHI::CreateDepthStencilStateState(const FRHIDepthStencilStateDesc& InDesc)
{
	return CreateRef<FVulkanDepthStencilState>(InDesc);
}

Ref<FRHIColorBlendState> FVulkanDynamicRHI::CreateColorBlendState(const FRHIColorBlendStateDesc& InDesc)
{
	return CreateRef<FVulkanColorBlendState>(InDesc);
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


