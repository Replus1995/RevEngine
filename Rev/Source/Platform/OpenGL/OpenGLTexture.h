#pragma once
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

	static Ref<FOpenGLTexture> Create(const FTextureDesc& InDesc);
protected:
	FOpenGLTexture(const FTextureDesc& InDesc) 
		: FRHITexture(InDesc) 
		, mFormatData(FOpenGLPixelFormat::TranslatePixelFormat(InDesc.Format))
	{
	}

protected:
	GLuint mHandle = 0;
	FOpenGLFormatData mFormatData;
};

}