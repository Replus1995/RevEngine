#pragma once
#include "VulkanTexture.h"

namespace Rev
{

class FVulkanTextureCube : public FVulkanTexture
{
public:
	FVulkanTextureCube(const FRHITextureDesc& InDesc);
	virtual ~FVulkanTextureCube();
	virtual void UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;

private:
	virtual void Init() override;
};

}