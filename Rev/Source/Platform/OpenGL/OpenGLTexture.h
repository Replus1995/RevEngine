#pragma once
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Core/Buffer.h"
#include "OpenGLPixelFormat.h"
#include <glad/gl.h>

namespace Rev
{
using FClearColorBuffer = FScopedBuffer<4 * sizeof(float)>;
class FOpenGLSampler;
class FOpenGLTexture : public FRHITexture
{
public:
	virtual ~FOpenGLTexture() = default;
	virtual const void* GetNativeHandle() const override { return &mHandle; }
	virtual const FRHISampler* GetSampler() const override;
	virtual void ClearMipData(uint8 InMipLevel) override;

	const FOpenGLFormatData& GetOpenGLFormatData() const{ return mFormatData; }

protected:
	FOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
	std::pair<uint32, uint32> CalculateMipSize2D(uint32 InMipLevel);
	std::tuple<uint32, uint32, uint32> CalculateMipSize3D(uint32 InMipLevel);
	void FillClearColor(FClearColorBuffer& OutBuffer);
protected:
	GLuint mHandle = 0;
	FOpenGLFormatData mFormatData;
	Ref<FOpenGLSampler> mSampler = nullptr;
};


Ref<FOpenGLTexture> CreateOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);

}