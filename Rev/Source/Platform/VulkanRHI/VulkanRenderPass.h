#pragma once
#include "Rev/Render/RHI/RHIRenderPass.h"
#include <vulkan/vulkan.h>

namespace Rev
{

class FVulkanRenderPass : public FRHIRenderPass
{
public:
	FVulkanRenderPass(const FRHIRenderPassDesc& InDesc);
	virtual ~FVulkanRenderPass();
	virtual const void* GetNativeHandle() const override { return RenderPass; }

	void PrepareForDraw();
	VkFramebuffer GetFramebuffer() const { return Framebuffer; }
	uint32 GetNumAttachments() const { return NumAttachments; }
	uint32 GetFrameWidth() const { return FrameWidth; }
	uint32 GetFrameHeight() const { return FrameHeight; }
	const VkClearValue* GetClearValues() const { return &mClearValues[0]; }

private:
	void Init();
	void Release();
	VkImageView CreateImageView(class FVulkanTexture* InTexture, int32 ArraySlice, uint8 MipIndex);

private:
	VkRenderPass RenderPass = VK_NULL_HANDLE;
	VkFramebuffer Framebuffer = VK_NULL_HANDLE;
	uint32 NumAttachments = 0;
	uint32 FrameWidth = 0;
	uint32 FrameHeight = 0;
	VkImageView FrameImageViews[(RTA_MaxColorAttachments + 1) * 2];
	VkClearValue mClearValues[(RTA_MaxColorAttachments + 1) * 2];
};


}