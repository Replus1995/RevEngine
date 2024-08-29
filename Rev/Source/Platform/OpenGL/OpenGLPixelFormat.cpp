#include "OpenGLPixelFormat.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

struct FOpenGLFormatMapping
{
	EPixelFormat RevFormat;
	FOpenGLFormatData OpenGLFormatData;
};

constexpr FOpenGLFormatMapping sOpenGLFormatMappings[] = {
	{ PF_Unknown,		{ 0, 0, 0, 0 } },
	{ PF_R32G32B32A32F, { GL_RGBA32F, GL_RGBA, GL_FLOAT, 4 * 4 } },
	{ PF_R8G8B8A8,		{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4 } },
	{ PF_R8,			{ GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1 } },
	{ PF_R16,			{ GL_R16, GL_RED, GL_UNSIGNED_SHORT, 2 } },
	{ PF_RGB8,			{ GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, 3 } },
	{ PF_DepthStencil,	{ GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 4} },
	{ PF_ShadowDepth,	{ GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, 4 } },
};



FOpenGLFormatData FOpenGLPixelFormat::TranslatePixelFormat(EPixelFormat InFormat, bool bSRGB)
{
	REV_CORE_ASSERT(InFormat != EPixelFormat::PF_Unknown);
	if (!bSRGB)
	{
		return sOpenGLFormatMappings[InFormat].OpenGLFormatData;
	}
	else
	{
		switch (InFormat)
		{
		case Rev::PF_R8G8B8A8:
			return { GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, 4 };
		case Rev::PF_RGB8:
			return { GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE, 3 };
		default:
			break;
		}
		REV_CORE_ASSERT(false, "Unknown SRGB format");
		return { 0, 0, 0, 0 };
	}
}

}