#pragma once
#include "Rev/Render/RHI/RHIRenderPass.h"
#include <vulkan/vulkan.h>

namespace Rev
{

class FVulkanRenderPass : public FRHIRenderPass
{
public:
	FVulkanRenderPass(const FRenderPassDesc& InDesc);
	virtual ~FVulkanRenderPass();
	virtual const void* GetNativeHandle() const override { return mRenderPass; }

	void CreateResource();
	void ReleaseResource();
private:
	VkRenderPass mRenderPass = VK_NULL_HANDLE;
};


}