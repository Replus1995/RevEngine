#pragma once
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "VulkanTexture.h"

namespace Rev
{

class FVulkanRenderTarget : public FRHIRenderTarget
{
public:
	FVulkanRenderTarget(const FRenderTargetDesc& InDesc);
	virtual ~FVulkanRenderTarget();
	virtual const void* GetNativeHandle() const override { return &mFramebuffer; }

	virtual void ResizeTargets(uint16 InWidth, uint16 InHeight) override;
	virtual void ClearTarget(ERenderTargetAttachment Index) override;
	virtual void ClearTargets() override;
	virtual const Ref<FRHITexture> GetTargetTexture(ERenderTargetAttachment Index) const override;

protected:
	bool IsEmptyTarget() const;
	void CreateResource();
	void ReleaseResource();
	Ref<FVulkanTexture> CreateColorTexture(const FColorTargetDesc& InDesc);
	Ref<FVulkanTexture> CreateDepthStencilTexture(const FDepthStencilTargetDesc& InDesc);
protected:

	struct FAttachment
	{
		uint8 MipLevel = 0;
		int32 ArrayIndex = -1;
		Ref<FVulkanTexture> Texture = nullptr;
	};

	VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
	FAttachment mColorAttachments[RTA_MaxColorAttachments];
	FAttachment mDepthStencilAttachment;
	bool mAttachmentsDirty = false;
};

}