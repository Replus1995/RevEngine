#pragma once
#include "OpenGLTexture.h"
#include <glad/gl.h>

namespace Rev
{

class FOpenGLTexture3D : public FOpenGLTexture
{
public:
	FOpenGLTexture3D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc = {});
	virtual ~FOpenGLTexture3D();

	virtual void UpdateData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, uint16 InDepth) override;

private:
	void CreateResource();
};


}