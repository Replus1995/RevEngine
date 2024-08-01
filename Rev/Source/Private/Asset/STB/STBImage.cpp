#include "STBImage.h"

#include <stb_image.h>
#include <stb_image_write.h>

namespace Rev
{

FSTBImage2D::FSTBImage2D()
	: mData(nullptr)
	, mWidth(0)
	, mHeight(0)
	, mChannels(0)
	, m16Bit(false)
{
}

FSTBImage2D::FSTBImage2D(uint8* InData, int InWidth, int InHeight, int InChannels, bool In16Bit)
	: mData(InData)
	, mWidth(InWidth)
	, mHeight(InHeight)
	, mChannels(InChannels)
	, m16Bit(In16Bit)
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
	, m16Bit(Other.m16Bit)
{
	Other.mData = nullptr;
	Other.mWidth = 0;
	Other.mHeight = 0;
	Other.mChannels = 0;
	Other.m16Bit = false;
}


FSTBImage2D FSTBImage::ImportImage2D(const FPath& InPath)
{
	std::string NativePath = InPath.ToNative();
	const char* NativePathC = NativePath.c_str();
	bool b16Bit = stbi_is_16_bit(NativePathC);
	//bool bHDR = stbi_is_hdr(NativePathC);

	int ImgWidth = 0, ImgHeight = 0, ImgChannels = 0;
	if (b16Bit)
	{
		if (uint16* data = stbi_load_16(NativePathC, &ImgWidth, &ImgHeight, &ImgChannels, 0); data)
		{
			return FSTBImage2D((uint8*)data, ImgWidth, ImgHeight, ImgChannels, b16Bit);
		}
	}
	else
	{
		if (uint8* data = stbi_load(NativePathC, &ImgWidth, &ImgHeight, &ImgChannels, 0); data)
		{
			return FSTBImage2D(data, ImgWidth, ImgHeight, ImgChannels, b16Bit);
		}
	}

	return FSTBImage2D();
}

}