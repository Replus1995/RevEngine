#pragma once
#include "Rev/Asset/AssetLibrary.h"

namespace Rev
{

class FSTBImage2D
{
public:
	FSTBImage2D();
	FSTBImage2D(uint8* InData, int InWidth, int InHeight, int InChannels, bool In16Bit);
	~FSTBImage2D();

	FSTBImage2D(const FSTBImage2D& Other) = delete;
	FSTBImage2D(FSTBImage2D&& Other) noexcept;

	const uint8* Data() const { return mData; }
	int Width() const { return mWidth; }
	int Height() const { return mHeight; }
	int Channels() const { return mChannels; }
	bool Is16Bit() const { return m16Bit; }
	bool Empty() const { return mData != nullptr; }

private:
	uint8* mData;
	int mWidth;
	int mHeight;
	int mChannels;
	bool m16Bit;
};


class FSTBImage
{
public:
	static FSTBImage2D ImportImage2D(const FPath& InPath);

};

}