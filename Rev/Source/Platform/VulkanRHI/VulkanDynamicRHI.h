#pragma once
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Core/VulkanInstance.h"

#include <vk_mem_alloc.h>

namespace Rev
{

class FVulkanDynamicRHI : public IDyanmicRHI
{
public:
	FVulkanDynamicRHI();
	virtual ~FVulkanDynamicRHI();

	virtual void Init() override;
	virtual void Cleanup() override;

	virtual Scope<FRHIContext> CreateContext() override;
	//Buffer
	virtual Ref<FRHIVertexBuffer> CreateVertexBuffer(uint32 InSize, bool bDynamic) override;
	virtual Ref<FRHIIndexBuffer> CreateIndexBuffer(uint32 InStride, uint32 InCount, bool bDynamic) override;

	virtual Ref<FRHIUniformBuffer> CreateUniformBuffer(uint32 InSize) override;

	//Primitive
	virtual Ref<FRHIPrimitive> CreatePrimitive(EPrimitiveTopology InTopology) override;

	//Texture
	virtual Ref<FRHITexture> CreateTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc) override;
	virtual Ref<FRHIRenderTarget> CreateRenderTarget(const FRenderTargetDesc& InDesc) override;

	//Pipeline
	virtual Ref<FRHIRenderPass> CreateRenderPass(const FRenderPassDesc& InDesc) override;

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
};


}