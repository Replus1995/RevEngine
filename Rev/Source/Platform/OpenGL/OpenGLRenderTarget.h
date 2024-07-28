#pragma once
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "OpenGLTexture2D.h"
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

	virtual void ClearTarget(ERenderTargetAttachment Index) override;
	virtual const Ref<FRHITexture> GetTargetTexture(ERenderTargetAttachment Index) const override;
	virtual void Recreate(uint32 InWidth, uint32 InHeight) override;

private:
	void CreateResource();
protected:
	GLuint mHandle = 0;
	Ref<FOpenGLTexture> mColorTextures[RTA_MaxColorAttachments];
	Ref<FOpenGLTexture> mDepthTexture;
	bool mUseExternalSource = false;
};

}