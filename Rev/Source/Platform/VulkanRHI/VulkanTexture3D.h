#pragma once
#include "VulkanTexture.h"

namespace Rev
{
class FVulkanTexture3D : public FVulkanTexture
{
public:
	FVulkanTexture3D(const FRHITextureDesc& InDesc);
	virtual ~FVulkanTexture3D();
	virtual void UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;

private:
	virtual void Init() override;
};
}
