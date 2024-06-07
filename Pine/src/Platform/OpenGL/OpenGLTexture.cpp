#include "pinepch.h"
#include "OpenGLTexture.h"
#include "Pine/Core/Assert.h"

#include <stb_image.h>

namespace Pine
{

OpenGLTexture2D::OpenGLTexture2D(const TextureDescription& desc)
	: mDesc(desc), mWidth(mDesc.Width), mHeight(mDesc.Height)
{
	InitFormatInfo(mDesc.Format);
	CreateTexture();
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
	: mFilePath(path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		//PE_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	}

	if (data)
	{
		mLoaded = true;

		mWidth = width;
		mHeight = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		mInternalFormat = internalFormat;
		mDataFormat = dataFormat;
		mDataType = GL_UNSIGNED_BYTE; //do not support float format

		PE_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		CreateTexture();
		glTextureSubImage2D(mHandle, 0, 0, 0, mWidth, mHeight, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &mHandle);
}

void OpenGLTexture2D::SetData(void* data, uint32_t size)
{
	uint32_t bpp = GetGLPixelSize(mInternalFormat);
	PE_CORE_ASSERT(size == mWidth * mHeight * bpp, "Data must be entire texture!");
	glTextureSubImage2D(mHandle, 0, 0, 0, mWidth, mHeight, mDataFormat, mDataType, data);
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
	glBindTextureUnit(slot, mHandle);
}

uint32_t OpenGLTexture2D::GetGLPixelSize(GLenum internalFormat)
{
	switch (internalFormat)
	{
	case GL_R8: return 1;
	case GL_RGB8: return 3;
	case GL_RGBA8: return 4;
	case GL_RGB32F: return 4 * 4;
	}
	PE_CORE_ASSERT(false);
	return 0;
}

void OpenGLTexture2D::InitFormatInfo(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::R8:
		mInternalFormat = GL_R8;
		mDataFormat = GL_RED;
		mDataType = GL_UNSIGNED_BYTE;
		break;
	case ImageFormat::RGB8:
		mInternalFormat = GL_RGB8;
		mDataFormat = GL_RGB;
		mDataType = GL_UNSIGNED_BYTE;
		break;
	case ImageFormat::RGBA8:
		mInternalFormat = GL_RGBA8;
		mDataFormat = GL_RGBA;
		mDataType = GL_UNSIGNED_BYTE;
		break;
	case ImageFormat::RGBA32F:
		mInternalFormat = GL_RGBA32F;
		mDataFormat = GL_RGBA;
		mDataType = GL_FLOAT;
		break;
	default:
		PE_CORE_ASSERT(false);
	}
}

void OpenGLTexture2D::CreateTexture()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &mHandle);
	glTextureStorage2D(mHandle, 1, mInternalFormat, mWidth, mHeight);

	glTextureParameteri(mHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(mHandle, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mHandle, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

}