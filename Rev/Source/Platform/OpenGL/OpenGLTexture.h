#pragma once
#include "Rev/Render/RHI/RHITexture.h"
#include "OpenGLPixelFormat.h"
#include <glad/gl.h>

namespace Rev
{
class FOpenGLSampler;
class FOpenGLTexture : public FRHITexture
{
public:
	virtual ~FOpenGLTexture() = default;
	virtual void* GetNativeHandle() override { return &mHandle; }
	virtual const FRHISampler* GetSampler() const override;
	virtual void Bind(uint32 InUnit) const override;

	static Ref<FOpenGLTexture> Create(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
protected:
	FOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
protected:
	GLuint mHandle = 0;
	FOpenGLFormatData mFormatData;
	Ref<FOpenGLSampler> mSampler = nullptr;
};

}