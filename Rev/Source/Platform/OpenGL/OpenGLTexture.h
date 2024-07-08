#pragma once
#include "Rev/Render/RHI/RHITexture.h"
#include <glad/glad.h>

namespace Rev
{

class FOpenGLTexture : public FRHITexture
{
public:
	virtual ~FOpenGLTexture() = default;
	virtual void* GetNativeHandle() override { return &mHandle; }

	static uint32 GetGLPixelSize(GLenum internalFormat);
	static Ref<FOpenGLTexture> Create(const FRHITextureDesc& InDesc);

protected:
	FOpenGLTexture(const FRHITextureDesc& InDesc) : FRHITexture(InDesc) {};

protected:
	GLuint mHandle = 0;
};

}