#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHISampler.h"
#include <string>

namespace Rev
{

enum class ETextureDimension
{
	Texture2D,
	Texture2DArray, //Unsupported
	Texture3D, //Unsupported
	TextureCube, //Unsupported
	TextureCubeArray //Unsupported
};

struct FTextureClearColor
{
	union 
	{
		Math::FLinearColor RGBA = Math::FLinearColor(0,0,0,1);
		struct
		{
			float Depth;
			uint32 Stencil;
		};
	};
	FTextureClearColor() {}
	FTextureClearColor(const FTextureClearColor& Other) : RGBA(Other.RGBA) {}
	FTextureClearColor(const Math::FLinearColor& InColor) : RGBA(InColor) {}
	FTextureClearColor(float InDepth, uint32 InStencil) : Depth(InDepth), Stencil(InStencil) {}

	FTextureClearColor& operator=(const FTextureClearColor& Other)
	{
		RGBA = Other.RGBA;
		return *this;
	}
};

struct FTextureDesc
{
	uint16 Width = 1;
	uint16 Height = 1;
	uint16 Depth = 1; //For 3D Texture
	uint16 ArraySize = 1; //For Texture Array
	ETextureDimension Dimension = ETextureDimension::Texture2D;
	EPixelFormat Format = PF_R8G8B8A8;
	bool bSRGB = false;
	uint8 NumMips = 1;
	uint8 NumSamples = 1; //For MSAA
	FTextureClearColor ClearColor;

	FTextureDesc() {}
	FTextureDesc(ETextureDimension InDimension, EPixelFormat InFormat, bool InSRGB,
		uint16 InWidth, uint16 InHeight, uint16 InDepth, uint16 InArraySize,
		const FTextureClearColor& InClearColor, uint8 InNumMips, uint8 InNumSamples
	)
		: Width(InWidth)
		, Height(InHeight)
		, Depth(InDepth)
		, ArraySize(InArraySize)
		, Dimension(InDimension)
		, Format(InFormat)
		, bSRGB(InSRGB)
		, NumMips(InNumMips)
		, NumSamples(InNumSamples)
		, ClearColor(InClearColor)
	{
	}

	FTextureDesc& operator=(const FTextureDesc& Other)
	{
		Width = Other.Width;
		Height = Other.Height;
		Depth = Other.Depth;
		ArraySize = Other.ArraySize;
		Dimension = Other.Dimension;
		Format = Other.Format;
		NumMips = Other.NumMips;
		NumSamples = Other.NumSamples;
		ClearColor = Other.ClearColor;
		return *this;
	}

	static FTextureDesc MakeTexture2D(uint16 InWidth, uint16 InHeight, EPixelFormat InFormat, bool InSRGB, const FTextureClearColor& InClearColor, uint8 InNumMips = 1, uint8 InNumSamples = 1)
	{
		return FTextureDesc(ETextureDimension::Texture2D, InFormat, InSRGB, InWidth, InHeight, 1, 1, InClearColor, InNumMips, InNumSamples);
	}
	
};

class REV_API FRHITexture : public FRHIResource
{
public:
	virtual ~FRHITexture() = default;
	const FTextureDesc& GetDesc() const { return mDesc; }
	uint32 GetWidth() const { return mDesc.Width; }
	uint32 GetHeight() const { return mDesc.Height; }
	EPixelFormat GetFormat() const { return mDesc.Format; }

	virtual const FRHISampler* GetSampler() const = 0;
	virtual void Bind(uint32 InUnit) const = 0;

	virtual void UpdateData(const void* InData, uint32 InSize, uint8 InMipLevel = 0, uint16 InArrayIndex = 0) = 0;
	virtual void ClearData() = 0;
protected:
	FRHITexture(const FTextureDesc& InDesc) : mDesc(InDesc) {}
protected:
	FTextureDesc mDesc;
};

}