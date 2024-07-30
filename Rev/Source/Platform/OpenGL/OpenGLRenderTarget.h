#pragma once
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "OpenGLTexture.h"
#include <glad/gl.h>

namespace Rev
{

class FOpenGLRenderTarget : public FRHIRenderTarget
{
public:
	FOpenGLRenderTarget(const FRenderTargetDesc& InDesc);
	virtual ~FOpenGLRenderTarget();
	virtual void* GetNativeHandle() override { return &mHandle; }

	virtual void Bind() override;
	virtual void Unbind() override;

	virtual void ResizeTargets(uint16 InWidth, uint16 InHeight) override;
	virtual void ClearTarget(ERenderTargetAttachment Index) override;
	virtual void ClearTargets() override;
	virtual const Ref<FRHITexture> GetTargetTexture(ERenderTargetAttachment Index) const override;

	virtual void Attach(ERenderTargetAttachment Index, const Ref<FRHITexture>& InTexture, uint8 InMipLevel, int32 InArrayIndex) override;
	virtual void Detach(ERenderTargetAttachment Index) override;
	virtual void DetachAll() override;

protected:
	bool IsEmptyTarget() const;
	void CreateResource();
	void ReleaseResource();
	Ref<FOpenGLTexture> CreateColorTexture(const FColorTargetDesc& InDesc);
	Ref<FOpenGLTexture> CreateDepthStencilTexture(const FDepthStencilTargetDesc& InDesc);
protected:
	static const uint16 sMaxRenderTargetSize;

	struct FAttachment
	{
		uint8 MipLevel = 0;
		int32 ArrayIndex = -1;
		Ref<FOpenGLTexture> Texture = nullptr;

	};

	GLuint mHandle = 0;
	FAttachment mColorAttachments[RTA_MaxColorAttachments];
	FAttachment mDepthStencilAttachment;
	bool mAttachmentsDirty = false;
};

}