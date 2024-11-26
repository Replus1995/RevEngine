#pragma once
#include "VulkanTexture.h"

namespace Rev
{

class FVulkanTexture2D : public FVulkanTexture
{
public:
	FVulkanTexture2D(const FRHITextureDesc& InDesc);
	virtual ~FVulkanTexture2D();
	virtual void UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;

private:
	virtual void Init() override;
};


class FVulkanTextureSwapchain : public FVulkanTexture
{
public:
	FVulkanTextureSwapchain(VkImage InSwapchainImage, VkFormat InFormat, uint16 InWidth, uint16 InHeight);
	virtual ~FVulkanTextureSwapchain();
	virtual void UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override {}
	virtual bool IsSwapchainTexture() const { return true; }

private:
	virtual void Init() override;
	virtual void Release() override;
};


}