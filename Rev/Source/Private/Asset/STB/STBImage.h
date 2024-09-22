#pragma once
#include "Rev/Asset/AssetLibrary.h"
#include "Rev/Render/PixelFormat.h"

namespace Rev
{

class FSTBImage2D
{
public:
	FSTBImage2D();
	FSTBImage2D(uint8* InData, int InWidth, int InHeight, int InChannels, int InPixelDepth);
	~FSTBImage2D();

	FSTBImage2D(const FSTBImage2D& Other) = delete;
	FSTBImage2D(FSTBImage2D&& Other) noexcept;

	FSTBImage2D& operator=(const FSTBImage2D& Other) = delete;
	FSTBImage2D& operator=(FSTBImage2D&& Other) noexcept;

	const uint8* Data() const { return mData; }
	int Width() const { return mWidth; }
	int Height() const { return mHeight; }
	int Channels() const { return mChannels; }
	int PixelDepth() const { return mPixelDepth; }
	EPixelFormat Format() const { return mFormat; }
	bool Valid() const { return mFormat != PF_Unknown; }

	static bool SameSizeAndFormat(const FSTBImage2D& InA, const FSTBImage2D& InB);

private:
	static EPixelFormat GetDesiredFormat(int InChannels, int InPixelDepth);

private:
	uint8* mData;
	int mWidth;
	int mHeight;
	int mChannels;
	int mPixelDepth; //8¡¢16¡¢32
	EPixelFormat mFormat;
};


class FSTBImage
{
public:
	static FSTBImage2D ImportImage2D(const FPath& InPath);

};

}