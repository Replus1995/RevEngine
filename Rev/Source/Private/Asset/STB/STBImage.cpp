#include "STBImage.h"
#include "Rev/HAL/FIleManager.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

namespace Rev
{

FSTBImage2D::FSTBImage2D()
	: mData(nullptr)
	, mWidth(0)
	, mHeight(0)
	, mChannels(0)
	, mPixelDepth(8)
	, mFormat(PF_Unknown)
{
}

FSTBImage2D::FSTBImage2D(uint8* InData, int InWidth, int InHeight, int InChannels, int InPixelDepth)
	: mData(InData)
	, mWidth(InWidth)
	, mHeight(InHeight)
	, mChannels(InChannels)
	, mPixelDepth(InPixelDepth)
	, mFormat(GetDesiredFormat(InChannels, InPixelDepth))
{
}

FSTBImage2D::~FSTBImage2D()
{
	if(mData)
		stbi_image_free(mData);
}

FSTBImage2D::FSTBImage2D(FSTBImage2D&& Other) noexcept
	: mData(Other.mData)
	, mWidth(Other.mWidth)
	, mHeight(Other.mHeight)
	, mChannels(Other.mChannels)
	, mPixelDepth(Other.mPixelDepth)
	, mFormat(Other.mFormat)
{
	Other.mData = nullptr;
	Other.mWidth = 0;
	Other.mHeight = 0;
	Other.mChannels = 0;
	Other.mPixelDepth = 8;
	Other.mFormat = PF_Unknown;
}

FSTBImage2D& FSTBImage2D::operator=(FSTBImage2D&& Other) noexcept
{
	mData = Other.mData;
	mWidth = Other.mWidth;
	mHeight = Other.mHeight;
	mChannels = Other.mChannels;
	mPixelDepth = Other.mPixelDepth;
	mFormat = Other.mFormat;

	Other.mData = nullptr;
	Other.mWidth = 0;
	Other.mHeight = 0;
	Other.mChannels = 0;
	Other.mPixelDepth = 8;
	Other.mFormat = PF_Unknown;
	return *this;
}

bool FSTBImage2D::SameSizeAndFormat(const FSTBImage2D& InA, const FSTBImage2D& InB)
{
	return InA.mWidth == InB.mWidth && InA.mHeight == InB.mHeight && InA.mFormat == InB.mFormat;
}

EPixelFormat FSTBImage2D::GetDesiredFormat(int InChannels, int InPixelDepth)
{
	switch (InChannels)
	{
	case 1:
	{
		switch (InPixelDepth)
		{
		case 8: return PF_R8;
		case 16: return PF_R16;
		default: break;
		}
		break;
	}
	case 2:
	{
		switch (InPixelDepth)
		{
		case 8: return PF_R8G8;
		case 16: return PF_R16G16;
		default: break;
		}
		break;
	}
	case 3:
	case 4:
	{
		switch (InPixelDepth)
		{
		case 8: return PF_R8G8B8A8;
		case 16: return PF_R8G8B8A8;
		default: break;
		}
		break;
	}
	default:
		break;
	}
	return PF_Unknown;
}


FSTBImage2D FSTBImage::ImportImage2D(const char* InName)
{
	FBuffer ImageMem;
	if (IFileManager::Get().LoadBinaryFile(InName, ImageMem))
	{
		bool b16Bit = stbi_is_16_bit_from_memory(ImageMem.Data(), ImageMem.Size());
		//bool bHDR = stbi_is_hdr_from_memory(ImageMem.Data(), ImageMem.Size());

		int ImgWidth = 0, ImgHeight = 0, ImgChannels = 0;
		stbi_info_from_memory(ImageMem.Data(), ImageMem.Size(), &ImgWidth, &ImgHeight, &ImgChannels);
		int ReqChannels = ImgChannels == 3 ? 4 : ImgChannels;

		if (b16Bit)
		{
			if (uint16* data = stbi_load_16_from_memory(ImageMem.Data(), ImageMem.Size(), &ImgWidth, &ImgHeight, &ImgChannels, ReqChannels); data)
			{
				return FSTBImage2D((uint8*)data, ImgWidth, ImgHeight, ReqChannels, 16);
			}
		}
		else
		{
			if (uint8* data = stbi_load_from_memory(ImageMem.Data(), ImageMem.Size(), &ImgWidth, &ImgHeight, &ImgChannels, ReqChannels); data)
			{
				return FSTBImage2D(data, ImgWidth, ImgHeight, ReqChannels, 8);
			}
		}

	}

	return FSTBImage2D();
}

}