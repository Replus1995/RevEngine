#pragma once
#include "Rev/Render/PixelFormat.h"
#include <glad/glad.h>

namespace Rev
{

struct FOpenGLFormatData
{
	GLenum InternalFormat;
	GLenum DataFormat;
	GLenum DataType;
	uint32 BytePerPixel;
};

class FOpenGLPixelFormat
{
public:
	static FOpenGLFormatData TranslatePixelFormat(EPixelFormat InFormat, bool bSRGB);
};




}