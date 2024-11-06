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
	virtual const FRHIDeviceCapacity& GetDeviceCapacity() override { return mDeviceCapacity; };

	virtual Scope<FRHIContext> RHICreateContext() override;
	//Buffer
	virtual Ref<FRHIBuffer> RHICreateBuffer(uint32 InSize, uint32 InStride, EBufferUsageFlags InFlags) override;
	virtual Ref<FRHIUniformBuffer> RHICreateUniformBuffer(uint32 InSize) override;

	//Texture
	virtual Ref<FRHITexture> RHICreateTexture(const FRHITextureDesc& InDesc) override;
	virtual void RHIResizeTexture(FRHITexture* InTexture, uint32 InWidth, uint32 InHeight, uint32 InDepth) override;

	//State
	virtual Ref<FRHISamplerState> RHICreateSamplerState(const FRHISamplerStateDesc& InDesc) override;
	virtual Ref<FRHIRasterizerState> RHICreateRasterizerState(const FRHIRasterizerStateDesc& InDesc) override;
	virtual Ref<FRHIDepthStencilState> RHICreateDepthStencilStateState(const FRHIDepthStencilStateDesc& InDesc) override;
	virtual Ref<FRHIColorBlendState> RHICreateColorBlendState(const FRHIColorBlendStateDesc& InDesc) override;
	virtual Ref<FRHIVertexInputState> RHICreateVertexInputState(const FRHIVertexInputStateDesc& InDesc) override;

	//Pipeline
	virtual Ref<FRHIRenderPass> RHICreateRenderPass(const FRHIRenderPassDesc& InDesc) override;

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
	FRHIDeviceCapacity mDeviceCapacity;

	std::map<uint32, Ref<FVulkanSamplerState>> mSamplerMap;
};


}