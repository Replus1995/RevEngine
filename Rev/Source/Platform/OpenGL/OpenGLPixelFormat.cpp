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
	{ PF_DepthStencil,	{ GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 4 } },
};



FOpenGLFormatData FOpenGLPixelFormat::TranslatePixelFormat(EPixelFormat InFormat)
{
	RE_CORE_ASSERT(InFormat != EPixelFormat::PF_Unknown);
	return sOpenGLFormatMappings[(uint8)InFormat].OpenGLFormatData;
}

}