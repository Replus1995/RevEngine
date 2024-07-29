#pragma once
#include "OpenGLTexture.h"
#include <glad/gl.h>

namespace Rev
{

class FOpenGLTextureCube : public FOpenGLTexture
{
public:
	FOpenGLTextureCube(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc = {});
	virtual ~FOpenGLTextureCube();

	virtual void UpdateData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, uint16 InDepth) override;

private:
	void CreateResource();
};


}