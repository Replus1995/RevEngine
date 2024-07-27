#include "OpenGLTexture2D.h"
#include "Rev/Core/Assert.h"

#include "OpenGLSampler.h"


namespace Rev
{

FOpenGLTexture2D::FOpenGLTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	CreateResource();
	mBindlessHandle = glGetTextureSamplerHandleARB(mHandle, *(GLuint*)mSampler->GetNativeHandle());
	glMakeTextureHandleResidentARB(mBindlessHandle);
}

FOpenGLTexture2D::~FOpenGLTexture2D()
{
	glMakeTextureHandleNonResidentARB(mBindlessHandle);
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTexture2D::UpdateData(const void* InData, uint32 InSize)
{
	RE_CORE_ASSERT(InSize == GetWidth() * GetHeight() * mFormatData.PixelSize, "Data must be entire texture!");
	glTextureSubImage2D(mHandle, 0, 0, 0, GetWidth(), GetHeight(), mFormatData.DataFormat, mFormatData.DataType, InData);
}

void FOpenGLTexture2D::ClearData()
{
	switch (mDesc.Format)
	{
	case PF_R32G32B32A32F:
	{
		const float* ClearColor = mDesc.ClearColor.RGBA.Data();
		glClearTexImage(mHandle, 0, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R8G8B8A8:
	case PF_RGB8:
	{
		Math::FColor ClearColor(0, 0, 0, 0);
		if (mDesc.bSRGB)
		{
			ClearColor = Math::FLinearColor::ToSRGB(mDesc.ClearColor.RGBA);
		}
		else
		{
			for (size_t i = 0; i < 4; i++)
			{
				ClearColor[i] = uint8(mDesc.ClearColor.RGBA[i] * 255.0f);
			}
		}
		glClearTexImage(mHandle, 0, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R8:
	{
		uint8 ClearColor = mDesc.ClearColor.RGBA.R * 255.0f;
		glClearTexImage(mHandle, 0, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R16:
	{
		uint16 ClearColor = mDesc.ClearColor.RGBA.R * 65535.0f;
		glClearTexImage(mHandle, 0, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_DepthStencil:
	{
		uint32 Depth = uint32(mDesc.ClearColor.Depth * 16777215.0f) << 8;
		uint8 Stencil = mDesc.ClearColor.Stencil;
		uint32 ClearColor = Depth & (uint32)Stencil;
		glClearTexImage(mHandle, 0, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_ShadowDepth:
	{
		float Depth = mDesc.ClearColor.Depth;
		glClearTexImage(mHandle, 0, mFormatData.DataFormat, mFormatData.DataType, (const void*)&Depth);
		break;
	}
	default:
		break;
	}
	
}

void FOpenGLTexture2D::CreateResource()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &mHandle);
	glTextureStorage2D(mHandle, 1, mFormatData.InternalFormat,  GetWidth(), GetHeight());
}

}