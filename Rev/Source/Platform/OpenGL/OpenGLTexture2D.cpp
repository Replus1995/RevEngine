#include "OpenGLTexture2D.h"
#include "Rev/Core/Assert.h"

//#include <stb_image.h>

namespace Rev
{

FOpenGLTexture2D::FOpenGLTexture2D(const FTextureDesc& InDesc)
	: FOpenGLTexture(InDesc)
{
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
	RE_CORE_ASSERT(InSize == GetWidth() * GetHeight() * mFormatData.BytePerPixel, "Data must be entire texture!");
	glTextureSubImage2D(mHandle, 0, 0, 0, GetWidth(), GetHeight(), mFormatData.DataFormat, mFormatData.DataType, InData);
}

void FOpenGLTexture2D::ClearData()
{
	switch (mDesc.Format)
	{
	case PF_R32G32B32A32F:
	{
		const float* ClearColor = mDesc.ClearColor.RGBA.Data();
		glClearTexImage(mHandle, 0, mFormatData.InternalFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R8G8B8A8:
	case PF_RGB8:
	{
		Math::FLinearColor RGBA8 = mDesc.ClearColor.RGBA * 255.0f;
		uint8 ClearColor[4] = { RGBA8.R, RGBA8.G, RGBA8.B, RGBA8.A };
		glClearTexImage(mHandle, 0, mFormatData.InternalFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R8:
	{
		uint8 ClearColor = mDesc.ClearColor.RGBA.R * 255.0f;
		glClearTexImage(mHandle, 0, mFormatData.InternalFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R16:
	{
		uint16 ClearColor = mDesc.ClearColor.RGBA.R * 65535.0f;
		glClearTexImage(mHandle, 0, mFormatData.InternalFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_DepthStencil:
	{
		uint32 Depth = uint32(mDesc.ClearColor.Depth * 16777215.0f) << 8;
		uint8 Stencil = mDesc.ClearColor.Stencil;
		uint32 ClearColor = Depth & (uint32)Stencil;
		glClearTexImage(mHandle, 0, mFormatData.InternalFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	default:
		break;
	}
	
}

void FOpenGLTexture2D::Bind(uint32 InUnit) const
{
	glBindTextureUnit(InUnit, mHandle);
}

void FOpenGLTexture2D::CreateResource()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &mHandle);
	glTextureStorage2D(mHandle, 1, mFormatData.InternalFormat,  GetWidth(), GetHeight());

	/*glTextureParameteri(mHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(mHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(mHandle, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(mHandle, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
}

}