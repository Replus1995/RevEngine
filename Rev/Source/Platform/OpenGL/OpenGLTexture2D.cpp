#include "OpenGLTexture2D.h"
#include "Rev/Core/Assert.h"

//#include <stb_image.h>

namespace Rev
{

FOpenGLTexture2D::FOpenGLTexture2D(const FTextureDesc& InDesc)
	: FOpenGLTexture(InDesc)
{
	InitFormatInfo(mDesc.Format);
	CreateResource();
}


	//int width, height, channels;
	//stbi_set_flip_vertically_on_load(1);
	//stbi_uc* data = nullptr;
	//{
	//	//PE_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
	//	data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	//}

	//if (data)
	//{
	//	mLoaded = true;

	//	mWidth = width;
	//	mHeight = height;

	//	GLenum internalFormat = 0, dataFormat = 0;
	//	if (channels == 4)
	//	{
	//		internalFormat = GL_RGBA8;
	//		dataFormat = GL_RGBA;
	//	}
	//	else if (channels == 3)
	//	{
	//		internalFormat = GL_RGB8;
	//		dataFormat = GL_RGB;
	//	}

	//	mInternalFormat = internalFormat;
	//	mDataFormat = dataFormat;
	//	mDataType = GL_UNSIGNED_BYTE; //do not support float format

	//	RE_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

	//	CreateTexture();
	//	glTextureSubImage2D(mHandle, 0, 0, 0, mWidth, mHeight, dataFormat, GL_UNSIGNED_BYTE, data);

	//	stbi_image_free(data);
	//}

FOpenGLTexture2D::~FOpenGLTexture2D()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTexture2D::UpdateData(const void* InData, uint32 InSize)
{
	uint32_t bpp = GetGLPixelSize(mInternalFormat);
	RE_CORE_ASSERT(InSize == GetWidth() * GetHeight() * bpp, "Data must be entire texture!");
	glTextureSubImage2D(mHandle, 0, 0, 0, GetWidth(), GetHeight(), mDataFormat, mDataType, InData);
}

void FOpenGLTexture2D::Bind(uint32 InUnit) const
{
	glBindTextureUnit(InUnit, mHandle);
}

void FOpenGLTexture2D::InitFormatInfo(EPixelFormat InFormat)
{
	switch (InFormat)
	{
	case PF_R32G32B32A32F:
		mInternalFormat = GL_RGBA32F;
		mDataFormat = GL_RGBA;
		mDataType = GL_FLOAT;
		break;
	case PF_R8G8B8A8:
		mInternalFormat = GL_RGBA8;
		mDataFormat = GL_RGBA;
		mDataType = GL_UNSIGNED_BYTE;
		break;
	case PF_R8:
		mInternalFormat = GL_R8;
		mDataFormat = GL_RED;
		mDataType = GL_UNSIGNED_BYTE;
		break;
	case PF_R16:
		mInternalFormat = GL_R8;
		mDataFormat = GL_RED;
		mDataType = GL_UNSIGNED_SHORT;
		break;
	case PF_RGB8:
		mInternalFormat = GL_RGB8;
		mDataFormat = GL_RGB;
		mDataType = GL_UNSIGNED_BYTE;
		break;
	default:
		RE_CORE_ASSERT(false);
	}
}

void FOpenGLTexture2D::CreateResource()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &mHandle);
	glTextureStorage2D(mHandle, 1, mInternalFormat,  GetWidth(), GetHeight());

	glTextureParameteri(mHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(mHandle, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mHandle, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

}