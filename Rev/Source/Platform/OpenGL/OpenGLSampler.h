#pragma once
#include "Rev/Render/RHI/RHISampler.h"
#include <glad/glad.h>

namespace Rev
{

class FOpenGLSampler : public FRHISampler
{
public:
	FOpenGLSampler(const FSamplerDesc& InDesc) : FRHISampler(InDesc) {}
	virtual ~FOpenGLSampler() {};
	virtual void* GetNativeHandle() override { return nullptr; }

	static void UpdateSampleState(const FSamplerDesc& InDesc, GLuint InTexHandle, bool bMipMap);

private:
	static GLenum TranslateFilterMode(ESamplerFilterMode InMode, bool bMipMap);
	static GLenum TranslateWarpMode(ESamplerWarpMode InMode);
	static bool UseAnisotropicFilter(const FSamplerDesc& InDesc);
	static bool UseBorderWarp(const FSamplerDesc& InDesc);
};

}