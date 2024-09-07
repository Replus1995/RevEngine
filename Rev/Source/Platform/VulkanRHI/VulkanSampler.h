#pragma once
#include "Rev/Render/RHI/RHISampler.h"
#include <vulkan/vulkan.h>

namespace Rev
{

class FVulkanSampler : public FRHISampler
{
public:
	FVulkanSampler(const FSamplerDesc& InDesc);
	virtual ~FVulkanSampler();
	virtual const void* GetNativeHandle() const override { return mSampler; }

private:
	static bool UseAnisotropicFilter(const FSamplerDesc& InDesc);
	static bool UseBorderWarp(const FSamplerDesc& InDesc);
	void CreateResource();

private:
	VkSampler mSampler = VK_NULL_HANDLE;
};

Ref<FVulkanSampler> CreateVkSampler(const FSamplerDesc& InSamplerDesc); //create or get from cache

}