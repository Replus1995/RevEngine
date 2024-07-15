#include "OpenGLSampler.h"

namespace Rev
{

void FOpenGLSampler::UpdateSampleState(const FSamplerDesc& InDesc, GLuint InTexHandle, bool bMipMap)
{
	glTextureParameteri(InTexHandle, GL_TEXTURE_MIN_FILTER, TranslateFilterMode(InDesc.Filter, bMipMap));
	glTextureParameteri(InTexHandle, GL_TEXTURE_MAG_FILTER, TranslateFilterMode(InDesc.Filter, false));

	glTextureParameteri(InTexHandle, GL_TEXTURE_WRAP_S, TranslateWarpMode(InDesc.WarpU));
	glTextureParameteri(InTexHandle, GL_TEXTURE_WRAP_T, TranslateWarpMode(InDesc.WarpV));
	glTextureParameteri(InTexHandle, GL_TEXTURE_WRAP_R, TranslateWarpMode(InDesc.WarpW));

	if (UseAnisotropicFilter(InDesc))
	{
		glTextureParameterf(InTexHandle, GL_TEXTURE_MAX_ANISOTROPY, InDesc.Anisotropic);
	}
	else
	{
		glTextureParameterf(InTexHandle, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);
	}

	/*if (UseBorderWarp(InDesc))
	{
		glTextureParameterfv(InTexHandle, GL_TEXTURE_BORDER_COLOR, mDesc.BorderColor.Data());
	}*/
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