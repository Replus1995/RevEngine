#pragma once
#include "OpenGLTexture.h"
#include <glad/glad.h>

namespace Rev
{

class FOpenGLTexture2D : public FOpenGLTexture
{
public:
	FOpenGLTexture2D(const FRHITextureDesc&  InDesc);
	virtual ~FOpenGLTexture2D();

	virtual void SetData(const void* InData, uint32 InSize) override;
	virtual void Bind(uint32 InSlot = 0) const override;

private:
	void InitFormatInfo(EPixelFormat InFormat);
	void CreateResource();

private:
	GLenum mInternalFormat = 0, mDataFormat = 0, mDataType = 0;
};


}