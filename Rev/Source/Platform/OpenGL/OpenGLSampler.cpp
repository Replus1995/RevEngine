#include "OpenGLSampler.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLSampler::FOpenGLSampler(const FSamplerDesc& InDesc, GLenum InTexDataType, bool bHasMip)
	: FRHISampler(InDesc)
	, mTexDataType(InTexDataType)
	, mHasMip(bHasMip)
{
	glCreateSamplers(1, &mHandle);
	FullUpdateState();
}

FOpenGLSampler::~FOpenGLSampler()
{
	glDeleteSamplers(1, &mHandle);
}

void FOpenGLSampler::SetFilterMode(ESamplerFilterMode InMode)
{
	if (mDesc.Filter != InMode)
	{
		mDesc.Filter = InMode;
		glSamplerParameteri(mHandle, GL_TEXTURE_MIN_FILTER, TranslateFilterMode(mDesc.Filter, mHasMip));
		glSamplerParameteri(mHandle, GL_TEXTURE_MAG_FILTER, TranslateFilterMode(mDesc.Filter, false));
	}
}

void FOpenGLSampler::SetAnisotropicMode(ESamplerAnisotropicMode InMode)
{
	if (mDesc.Anisotropic != InMode)
	{
		mDesc.Anisotropic = InMode;
		if (UseAnisotropicFilter(mDesc))
			glSamplerParameteri(mHandle, GL_TEXTURE_MAX_ANISOTROPY, mDesc.Anisotropic);
		else
			glSamplerParameteri(mHandle, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);
	}
}

void FOpenGLSampler::SetWarpU(ESamplerWarpMode InMode)
{
	if (mDesc.WarpU != InMode)
	{
		mDesc.WarpU = InMode;
		glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_S, TranslateWarpMode(mDesc.WarpU));
		if (UseBorderWarp(mDesc))
			glSamplerParameterfv(mHandle, GL_TEXTURE_BORDER_COLOR, mDesc.BorderColor.Data());
	}
}

void FOpenGLSampler::SetWarpV(ESamplerWarpMode InMode)
{
	if (mDesc.WarpV != InMode)
	{
		mDesc.WarpV = InMode;
		glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_T, TranslateWarpMode(mDesc.WarpV));
		if (UseBorderWarp(mDesc))
			glSamplerParameterfv(mHandle, GL_TEXTURE_BORDER_COLOR, mDesc.BorderColor.Data());
	}
}

void FOpenGLSampler::SetWarpW(ESamplerWarpMode InMode)
{
	if (mDesc.WarpW != InMode)
	{
		mDesc.WarpW = InMode;
		glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_R, TranslateWarpMode(mDesc.WarpW));
		if (UseBorderWarp(mDesc))
			glSamplerParameterfv(mHandle, GL_TEXTURE_BORDER_COLOR, mDesc.BorderColor.Data());
	}
}

void FOpenGLSampler::SetBorderColor(const Math::FLinearColor& InColor)
{
	if (mDesc.BorderColor != InColor)
	{
		mDesc.BorderColor = InColor;
		if (UseBorderWarp(mDesc))
			glSamplerParameterfv(mHandle, GL_TEXTURE_BORDER_COLOR, mDesc.BorderColor.Data());
	}
	
}

void FOpenGLSampler::FullUpdateState()
{
	glSamplerParameteri(mHandle, GL_TEXTURE_MIN_FILTER, TranslateFilterMode(mDesc.Filter, mHasMip));
	glSamplerParameteri(mHandle, GL_TEXTURE_MAG_FILTER, TranslateFilterMode(mDesc.Filter, false));

	glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_S, TranslateWarpMode(mDesc.WarpU));
	glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_T, TranslateWarpMode(mDesc.WarpV));
	glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_R, TranslateWarpMode(mDesc.WarpW));

	if (UseAnisotropicFilter(mDesc))
	{
		glSamplerParameteri(mHandle, GL_TEXTURE_MAX_ANISOTROPY, mDesc.Anisotropic);
	}
	else
	{
		glSamplerParameteri(mHandle, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);
	}

	if (UseBorderWarp(mDesc))
	{
		glSamplerParameterfv(mHandle, GL_TEXTURE_BORDER_COLOR, mDesc.BorderColor.Data());
	}
}

GLenum FOpenGLSampler::TranslateFilterMode(ESamplerFilterMode InMode, bool bMipMap)
{

	if (bMipMap)
	{
		switch (InMode)
		{
		case SF_Nearest:
			return GL_NEAREST_MIPMAP_NEAREST;
		case SF_Bilinear:
			return GL_LINEAR_MIPMAP_NEAREST;
		case SF_Trilinear:
			return GL_LINEAR_MIPMAP_LINEAR;
		case SF_AnisotropicNearest:
			return GL_LINEAR_MIPMAP_NEAREST;
		case SF_AnisotropicLinear:
			return GL_LINEAR_MIPMAP_LINEAR;
		}
	}
	else
	{
		switch (InMode)
		{
		case SF_Nearest:
			return GL_NEAREST;
		case SF_Bilinear:
		case SF_Trilinear:
		case SF_AnisotropicNearest:
		case SF_AnisotropicLinear:
			return GL_LINEAR;
		}
	}
	REV_CORE_ASSERT(false, "Invalid filter mode");
	return GLenum(0);
}

GLenum FOpenGLSampler::TranslateWarpMode(ESamplerWarpMode InMode)
{
	switch (InMode)
	{
	case Rev::SW_Repeat:
		return GL_REPEAT;
	case Rev::SW_Clamp:
		return GL_CLAMP_TO_EDGE;
	case Rev::SW_Mirror:
		return GL_MIRRORED_REPEAT;
	case Rev::SW_Border:
		return GL_CLAMP_TO_BORDER;
	default:
		break;
	}

	return GL_REPEAT;
}

bool FOpenGLSampler::UseAnisotropicFilter(const FSamplerDesc& InDesc)
{
	return InDesc.Filter >= SF_AnisotropicNearest;
}

bool FOpenGLSampler::UseBorderWarp(const FSamplerDesc& InDesc)
{
	return InDesc.WarpU == SW_Border || InDesc.WarpV == SW_Border || InDesc.WarpW == SW_Border;
}

}