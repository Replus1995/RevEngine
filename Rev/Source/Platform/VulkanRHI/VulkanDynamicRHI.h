#pragma once
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Core/VulkanInstance.h"

#include <map>
#include <vk_mem_alloc.h>

namespace Rev
{

class FVulkanSamplerState;

class FVulkanDynamicRHI : public IDyanmicRHI
{
public:
	FVulkanDynamicRHI();
	virtual ~FVulkanDynamicRHI();

	virtual void Init() override;
	virtual void Cleanup() override;

	virtual Scope<FRHIContext> RHICreateContext() override;
	//Buffer
	virtual Ref<FRHIVertexBuffer> CreateVertexBuffer(uint32 InSize, bool bDynamic) override;
	virtual Ref<FRHIIndexBuffer> CreateIndexBuffer(uint32 InStride, uint32 InCount, bool bDynamic) override;
	virtual Ref<FRHIUniformBuffer> CreateUniformBuffer(uint32 InSize) override;
	virtual Ref<FRHIBuffer> RHICreateBuffer(uint32 InSize, uint32 InStride, EBufferUsageFlags InFlags) override {};

	//Primitive
	virtual Ref<FRHIPrimitive> CreatePrimitive(EPrimitiveTopology InTopology) override;

	//Texture
	virtual Ref<FRHITexture> CreateTexture(const FRHITextureDesc& InDesc ) override;
	virtual Ref<FRHIRenderTarget> CreateRenderTarget(const FRenderTargetDesc& InDesc) override;

	//State
	virtual Ref<FRHISamplerState> RHICreateSamplerState(const FRHISamplerStateDesc& InDesc) override;
	virtual Ref<FRHIRasterizerState> RHICreateRasterizerState(const FRHIRasterizerStateDesc& InDesc) override;
	virtual Ref<FRHIDepthStencilState> RHICreateDepthStencilStateState(const FRHIDepthStencilStateDesc& InDesc) override;
	virtual Ref<FRHIColorBlendState> RHICreateColorBlendState(const FRHIColorBlendStateDesc& InDesc) override;
	virtual Ref<FRHIVertexInputState> RHICreateVertexInputState(const FRHIVertexInputStateDesc& InDesc) override;

	//Pipeline
	virtual Ref<FRHIRenderPass> CreateRenderPass(const FRHIRenderPassDesc& InDesc) override;

public:
	static FVulkanDynamicRHI* GetVulkanRHI();
	static VkInstance GetInstance() { return GetVulkanRHI()->mInstance.GetInstance(); };
	static VkSurfaceKHR GetSurface() { return GetVulkanRHI()->mInstance.GetSurface(); }

	static VkPhysicalDevice GetPhysicalDevice() { return GetVulkanRHI()->mInstance.GetPhysicalDevice(); }
	static VkDevice GetDevice() { return GetVulkanRHI()->mInstance.GetDevice(); }
	static VkQueue GetQueue(EVulkanQueueKind InKind) { return GetVulkanRHI()->mInstance.GetQueue(InKind); }
	static uint32 GetQueueFamily(EVulkanQueueKind InKind) { return GetVulkanRHI()->mInstance.GetQueueFamily(InKind); }
	static const FVulkanSurfaceSupport& GetSurfaceSupport() { return GetVulkanRHI()->mInstance.GetSurfaceSupport(); }
	static VmaAllocator GetAllocator() { return GetVulkanRHI()->mAllocator; }

private:
	FVulkanInstance mInstance;
	VmaAllocator mAllocator = nullptr;

	std::map<uint32, Ref<FVulkanSamplerState>> mSamplerMap;
};


}