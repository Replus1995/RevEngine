#include "OpenGLSampler.h"

namespace Rev
{

void FOpenGLSampler::SyncSampleState(GLuint InTexHandle, bool bMipMap)
{
	glTextureParameteri(InTexHandle, GL_TEXTURE_MIN_FILTER, TranslateMinFilterMode(mDesc.Filter, bMipMap));
	glTextureParameteri(InTexHandle, GL_TEXTURE_MAG_FILTER, TranslateMagFilterMode(mDesc.Filter));

	glTextureParameteri(InTexHandle, GL_TEXTURE_WRAP_S, TranslateWarpMode(mDesc.WarpU));
	glTextureParameteri(InTexHandle, GL_TEXTURE_WRAP_T, TranslateWarpMode(mDesc.WarpV));
	glTextureParameteri(InTexHandle, GL_TEXTURE_WRAP_R, TranslateWarpMode(mDesc.WarpW));

	if (UseAnisotropicFilter())
	{
		glTextureParameterf(InTexHandle, GL_TEXTURE_MAX_ANISOTROPY, mDesc.Anisotropic);
	}
	else
	{
		glTextureParameterf(InTexHandle, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);
	}

	if (UseBorderWarp())
	{
		glTextureParameterfv(InTexHandle, GL_TEXTURE_BORDER_COLOR, mDesc.BorderColor.Data());
	}
}

GLenum FOpenGLSampler::TranslateMinFilterMode(ESamplerFilterMode InMode, bool bMipMap)
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
}


GLenum FOpenGLSampler::TranslateMagFilterMode(ESamplerFilterMode InMode)
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

bool FOpenGLSampler::UseAnisotropicFilter() const
{
	return mDesc.Filter >= SF_AnisotropicNearest;
}

bool FOpenGLSampler::UseBorderWarp() const
{
	return mDesc.WarpU == SW_Border || mDesc.WarpV == SW_Border || mDesc.WarpW == SW_Border;
}

}