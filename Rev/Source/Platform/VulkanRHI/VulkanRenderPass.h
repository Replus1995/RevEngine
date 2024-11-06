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
	virtual const void* GetNativeHandle() const override { return mRenderPass; }
	virtual void MarkFramebufferDirty() override;

	void PrepareForDraw();
	VkFramebuffer GetFramebuffer() const { return mFramebuffer; }
	uint32 GetNumAttachments() const { return mNumAttachments; }
	uint32 GetFrameWidth() const { return mFrameWidth; }
	uint32 GetFrameHeight() const { return mFrameHeight; }
	VkSampleCountFlagBits GetSampleCountFlag() const { return mSampleCountFlag; }
	const VkClearValue* GetClearValues() const { return &mClearValues[0]; }

private:
	void Init();
	void Release();
private:
	VkRenderPass mRenderPass = VK_NULL_HANDLE;
	VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
	bool bFramebufferDirty = true;
	uint32 mNumAttachments = 0;
	uint32 mFrameWidth = 0;
	uint32 mFrameHeight = 0;
	VkSampleCountFlagBits mSampleCountFlag = VK_SAMPLE_COUNT_1_BIT;
	VkClearValue mClearValues[(RTA_MaxColorAttachments + 1) * 2];
};


}