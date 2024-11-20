#pragma once
#include "VulkanTexture.h"

namespace Rev
{

class FVulkanTexture2DArray : public FVulkanTexture
{
public:
	FVulkanTexture2DArray(const FRHITextureDesc& InDesc);
	virtual ~FVulkanTexture2DArray();
	virtual void UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;

private:
	virtual void Init() override;
};


}