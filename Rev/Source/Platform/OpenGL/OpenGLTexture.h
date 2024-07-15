#pragma once
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "OpenGLPixelFormat.h"
#include <glad/glad.h>

namespace Rev
{

class FOpenGLTexture : public FRHITexture
{
public:
	virtual ~FOpenGLTexture() = default;
	virtual void* GetNativeHandle() override { return &mHandle; }
	virtual void SetSamplerState(const FSamplerDesc& InSamplerDesc);
	static Ref<FOpenGLTexture> Create(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
protected:
	FOpenGLTexture(const FTextureDesc& InDesc);
	FOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
protected:
	GLuint mHandle = 0;
	FOpenGLFormatData mFormatData;
	FSamplerDesc mSamplerDesc;
};

}