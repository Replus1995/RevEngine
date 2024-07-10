#pragma once
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHISampler.h"
#include <glad/glad.h>

namespace Rev
{

class FOpenGLSampler : public FRHISamplerState
{
public:
	FOpenGLSampler(const FSamplerDesc& InDesc) : FRHISamplerState(InDesc) {}
	virtual ~FOpenGLSampler() {};
	virtual void* GetNativeHandle() override { return nullptr; }
};

class FOpenGLTexture : public FRHITexture
{
public:
	virtual ~FOpenGLTexture() = default;
	virtual void* GetNativeHandle() override { return &mHandle; }

	static uint32 GetGLPixelSize(GLenum internalFormat);
	static Ref<FOpenGLTexture> Create(const FTextureDesc& InDesc);

protected:
	FOpenGLTexture(const FTextureDesc& InDesc) : FRHITexture(InDesc) {};

protected:
	GLuint mHandle = 0;
};

}