#pragma once
#include "OpenGLTexture.h"
#include <glad/gl.h>

namespace Rev
{

class FOpenGLTextureCubeArray : public FOpenGLTexture
{
public:
	FOpenGLTextureCubeArray(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc = {});
	virtual ~FOpenGLTextureCubeArray();

	virtual void UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth) override;
	virtual void ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth) override;

private:
	void CreateResource();
};


}