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

	void UpdateSampleState(GLuint InTexHandle, bool bMipMap);

private:
	static GLenum TranslateMinFilterMode(ESamplerFilterMode InMode, bool bMipMap);
	static GLenum TranslateMagFilterMode(ESamplerFilterMode InMode);
	static GLenum TranslateWarpMode(ESamplerWarpMode InMode);
	bool UseAnisotropicFilter() const;
	bool UseBorderWarp() const;
};

}