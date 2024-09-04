#pragma once
#include "VulkanTexture.h"

namespace Rev
{

class FVulkanTexture2D : public FVulkanTexture
{
public:
	FVulkanTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc = {});
	virtual ~FVulkanTexture2D();

	virtual void UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth) override;
	virtual void ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth) override;

private:
	void CreateResource();
};


}