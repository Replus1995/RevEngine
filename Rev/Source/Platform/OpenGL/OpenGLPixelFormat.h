#pragma once
#include "Rev/Render/PixelFormat.h"
#include <glad/gl.h>

namespace Rev
{

struct FOpenGLFormatData
{
	GLenum InternalFormat;
	GLenum DataFormat;
	GLenum DataType;
	uint32 PixelSize;
};

class FOpenGLPixelFormat
{
public:
	static FOpenGLFormatData TranslatePixelFormat(EPixelFormat InFormat, bool bSRGB);
};




}