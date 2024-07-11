#pragma once
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "OpenGLTexture2D.h"
#include <glad/glad.h>

namespace Rev
{

class FOpenGLRenderTarget2D : public FRHIRenderTarget
{
public:
	FOpenGLRenderTarget2D(const FRenderTargetDesc& InDesc);
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
	std::vector<Ref<FOpenGLTexture2D>> mColorTextures;
	Ref<FOpenGLTexture2D> mDepthTexture;
};

}