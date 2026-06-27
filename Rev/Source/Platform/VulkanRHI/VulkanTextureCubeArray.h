#pragma once
#include "VulkanTexture.h"

namespace Rev
{

class FVulkanTextureCubeArray : public FVulkanTexture
{
public:
	FVulkanTextureCubeArray(const FRHITextureDesc& InDesc);
	virtual ~FVulkanTextureCubeArray();
	virtual void UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;

private:
	virtual void Init() override;
};


}
