#pragma once
#include "VkTexture.h"
#include <Vulkan/vulkan.h>

namespace Rev
{

class FVkTexture2D : public FVkTexture
{
public:
	FVkTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc = {});
	virtual ~FVkTexture2D();

	virtual void UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth) override;
	virtual void ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth) override;

private:
	void CreateResource();
};


}