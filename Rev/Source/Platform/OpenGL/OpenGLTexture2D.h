#pragma once
#include "OpenGLTexture.h"
#include <glad/gl.h>

namespace Rev
{

class FOpenGLTexture2D : public FOpenGLTexture
{
public:
	FOpenGLTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc = {});
	virtual ~FOpenGLTexture2D();

	virtual void UpdateData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;

private:
	void CreateResource();
};


}