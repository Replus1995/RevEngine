#pragma once
#include "Rev/Math/Maths.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"

namespace Rev
{

struct FRHITextureClearColor
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
	FRHITextureClearColor() {}
	FRHITextureClearColor(const FRHITextureClearColor& Other) : RGBA(Other.RGBA) {}
	FRHITextureClearColor(const Math::FLinearColor& InColor) : RGBA(InColor) {}
	FRHITextureClearColor(float InDepth, uint32 InStencil) : Depth(InDepth), Stencil(InStencil) {}

	FRHITextureClearColor& operator=(const FRHITextureClearColor& Other)
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
	uint8 NumMips = 1;
	uint8 NumSamples = 1; //For MSAA
	ETextureCreateFlags Flags = ETextureCreateFlags::None;
	FRHITextureClearColor ClearColor;

	FTextureDesc() {}

	FTextureDesc(ETextureDimension InDimension, EPixelFormat InFormat)
		: Dimension(InDimension)
		, Format(InFormat)
	{
	}

	/*FTextureDesc(
		ETextureDimension InDimension, 
		ETextureCreateFlags InFlags,
		EPixelFormat InFormat,
		const FRHITextureClearColor& InClearColor,
		uint16 InWidth, 
		uint16 InHeight, 
		uint16 InDepth, 
		uint16 InArraySize,
		uint8 InNumMips, 
		uint8 InNumSamples
	)
		: Width(InWidth)
		, Height(InHeight)
		, Depth(InDepth)
		, ArraySize(InArraySize)
		, Dimension(InDimension)
		, Format(InFormat)
		, NumMips(InNumMips)
		, NumSamples(InNumSamples)
		, Flags(InFlags)
		, ClearColor(InClearColor)
	{
	}*/

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
		Flags = Other.Flags;
		ClearColor = Other.ClearColor;
		return *this;
	}

	FTextureDesc& SetFlags(ETextureCreateFlags InFlags) { Flags = InFlags; return *this; }
	FTextureDesc& AddFlags(ETextureCreateFlags InFlags) { Flags |= InFlags; return *this; }
	FTextureDesc& SetExtent(uint16 InWidth, uint16 InHeight, uint16 InDepth = 1) { Width = InWidth; Height = InHeight; Depth = InDepth; return *this; }
	FTextureDesc& SetArraySize(uint16 InArraySize) { ArraySize = InArraySize; return *this; }
	FTextureDesc& SetClearColor(const FRHITextureClearColor& InClearColor) { ClearColor = InClearColor; return *this; }
	FTextureDesc& SetNumMips(uint8 InNumMips) { NumMips = InNumMips; return *this; }
	FTextureDesc& SetNumSamples(uint8 InNumSamples) { NumSamples = InNumSamples; return *this; }


	static FTextureDesc Make2D(uint16 InWidth, uint16 InHeight, EPixelFormat InFormat)
	{
		FTextureDesc Desc(ETextureDimension::Texture2D, InFormat);
		return Desc.SetExtent(InWidth, InHeight);
	}
	
	static FTextureDesc Make2DArray(uint16 InWidth, uint16 InHeight, uint16 InArraySize, EPixelFormat InFormat)
	{
		FTextureDesc Desc(ETextureDimension::Texture2DArray, InFormat);
		return Desc.SetExtent(InWidth, InHeight).SetArraySize(InArraySize);
	}

	static FTextureDesc MakeCube(uint16 InWidth, uint16 InHeight, EPixelFormat InFormat)
	{
		FTextureDesc Desc(ETextureDimension::TextureCube, InFormat);
		return Desc.SetExtent(InWidth, InHeight);
	}

	static FTextureDesc MakeCubeArray(uint16 InWidth, uint16 InHeight, uint16 InArraySize, EPixelFormat InFormat)
	{
		FTextureDesc Desc(ETextureDimension::TextureCubeArray, InFormat);
		return Desc.SetExtent(InWidth, InHeight).SetArraySize(InArraySize);
	}

	static FTextureDesc Make3D(uint16 InWidth, uint16 InHeight, uint16 InDepth, EPixelFormat InFormat)
	{
		FTextureDesc Desc(ETextureDimension::Texture3D, InFormat);
		return Desc.SetExtent(InWidth, InHeight, InDepth);
	}

};

class REV_API FRHITexture : public FRHIResource
{
public:
	FRHITexture(const FTextureDesc& InDesc) : mDesc(InDesc) {}
	virtual ~FRHITexture() = default;

	const FTextureDesc& GetDesc() const { return mDesc; }
	uint16 GetWidth() const { return mDesc.Width; }
	uint16 GetHeight() const { return mDesc.Height; }
	uint16 GetDepth() const { return mDesc.Depth; }
	uint16 GetArraySize() const { return mDesc.ArraySize; }
	EPixelFormat GetFormat() const { return mDesc.Format; }
	FPixelFormatInfo GetFormatInfo() const { return GPixelFormats[mDesc.Format]; }

protected:
	FTextureDesc mDesc;
};

}