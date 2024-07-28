#pragma once
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "OpenGLTexture2D.h"
#include <glad/gl.h>

namespace Rev
{

class FOpenGLRenderTarget2D : public FRHIRenderTarget
{
public:
	FOpenGLRenderTarget2D(const FRenderTargetDesc& InDesc);
	FOpenGLRenderTarget2D(const FRenderTargetSource* InSources, uint32 InNumSources);
	virtual ~FOpenGLRenderTarget2D();
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
	Ref<FOpenGLTexture2D> mColorTextures[RTA_MaxColorAttachments];
	Ref<FOpenGLTexture2D> mDepthTexture;
	bool mUseExternalSource = false;
};

}