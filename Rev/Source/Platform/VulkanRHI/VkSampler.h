#pragma once
#include "Rev/Render/RHI/RHISampler.h"
#include <vulkan/vulkan.h>

namespace Rev
{

class FVkSampler : public FRHISampler
{
public:
	FVkSampler(const FSamplerDesc& InDesc);
	virtual ~FVkSampler();
	virtual const void* GetNativeHandle() const override { return mSampler; }

private:
	static VkFilter TranslateFilterMode(ESamplerFilterMode InMode);
	static VkSamplerMipmapMode TranslateMipmapMode(ESamplerFilterMode InMode);
	static VkSamplerAddressMode TranslateWarpMode(ESamplerWarpMode InMode);
	static bool UseAnisotropicFilter(const FSamplerDesc& InDesc);
	static bool UseBorderWarp(const FSamplerDesc& InDesc);
	void CreateResource();

private:
	VkSampler mSampler = VK_NULL_HANDLE;
};

Ref<FVkSampler> CreateVkSampler(const FSamplerDesc& InSamplerDesc); //create or get from cache

}