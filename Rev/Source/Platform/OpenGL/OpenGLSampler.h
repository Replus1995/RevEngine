#pragma once
#include "Rev/Render/RHI/RHISampler.h"
#include <glad/glad.h>

namespace Rev
{
class FOpenGLSampler : public FRHISampler
{
public:
	FOpenGLSampler(const FSamplerDesc& InDesc, GLenum InTexDataType, bool bHasMip);
	virtual ~FOpenGLSampler();
	virtual void* GetNativeHandle() override { return &mHandle; }
	virtual void SetFilterMode(ESamplerFilterMode InMode) override;
	virtual void SetAnisotropicMode(ESamplerAnisotropicMode InMode) override;
	virtual void SetWarpU(ESamplerWarpMode InMode) override;
	virtual void SetWarpV(ESamplerWarpMode InMode) override;
	virtual void SetWarpW(ESamplerWarpMode InMode) override;
	virtual void SetBorderColor(const Math::FLinearColor& InColor) override;

private:
	void FullUpdateState();
private:
	GLuint mHandle = 0;
	GLenum mTexDataType = GL_UNSIGNED_BYTE;
	bool mHasMip = false;

private:
	static GLenum TranslateFilterMode(ESamplerFilterMode InMode, bool bMipMap);
	static GLenum TranslateWarpMode(ESamplerWarpMode InMode);
	static bool UseAnisotropicFilter(const FSamplerDesc& InDesc);
	static bool UseBorderWarp(const FSamplerDesc& InDesc);
};

}