#pragma once
#include "VulkanTexture.h"

namespace Rev
{

class FVulkanTexture2D : public FVulkanTexture
{
public:
	FVulkanTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc = {});
	FVulkanTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc, VkImageUsageFlags InUsageFlags);
	virtual ~FVulkanTexture2D();

	virtual void UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;

private:
	void CreateResource(VkImageUsageFlags InUsageFlags);
};


}