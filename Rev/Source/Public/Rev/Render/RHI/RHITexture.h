#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHISampler.h"

namespace Rev
{

enum class ETextureDimension : uint8
{
	Texture2D			= 1,
	Texture2DArray		= 2,
	TextureCube			= 3,
	TextureCubeArray	= 4,
	Texture3D			= 5,
};
 
enum ETextureCubeFace : uint8
{
	TCF_PositiveX = 0,
	TCF_NegativeX = 1,
	TCF_PositiveY = 2,
	TCF_NegativeY = 3,
	TCF_PositiveZ = 4,
	TCF_NegativeZ = 5,
	TCF_Count = 6
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
	EPixelFormat Format = PF_RGBA8;
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

	static FTextureDesc Make2D(uint16 InWidth, uint16 InHeight, EPixelFormat InFormat, bool InSRGB, const FTextureClearColor& InClearColor = {}, uint8 InNumMips = 1, uint8 InNumSamples = 1)
	{
		return FTextureDesc(ETextureDimension::Texture2D, InFormat, InSRGB, InWidth, InHeight, 1, 1, InClearColor, InNumMips, InNumSamples);
	}
	
	static FTextureDesc Make2DArray(uint16 InWidth, uint16 InHeight, EPixelFormat InFormat, bool InSRGB, uint16 InArraySize, const FTextureClearColor& InClearColor = {}, uint8 InNumMips = 1, uint8 InNumSamples = 1)
	{
		return FTextureDesc(ETextureDimension::Texture2DArray, InFormat, InSRGB, InWidth, InHeight, 1, InArraySize, InClearColor, InNumMips, InNumSamples);
	}

	static FTextureDesc MakeCube(uint16 InWidth, uint16 InHeight, EPixelFormat InFormat, bool InSRGB, const FTextureClearColor& InClearColor = {}, uint8 InNumMips = 1, uint8 InNumSamples = 1)
	{
		return FTextureDesc(ETextureDimension::TextureCube, InFormat, InSRGB, InWidth, InHeight, 1, 1, InClearColor, InNumMips, InNumSamples);
	}

	static FTextureDesc MakeCubeArray(uint16 InWidth, uint16 InHeight, EPixelFormat InFormat, bool InSRGB, uint16 InArraySize, const FTextureClearColor& InClearColor = {}, uint8 InNumMips = 1, uint8 InNumSamples = 1)
	{
		return FTextureDesc(ETextureDimension::TextureCubeArray, InFormat, InSRGB, InWidth, InHeight, 1, InArraySize, InClearColor, InNumMips, InNumSamples);
	}

	static FTextureDesc Make3D(uint16 InWidth, uint16 InHeight, uint16 InDepth, EPixelFormat InFormat, bool InSRGB, const FTextureClearColor& InClearColor = {}, uint8 InNumMips = 1, uint8 InNumSamples = 1)
	{
		return FTextureDesc(ETextureDimension::Texture3D, InFormat, InSRGB, InWidth, InHeight, InDepth, 1, InClearColor, InNumMips, InNumSamples);
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

	virtual const FRHISampler* GetSampler() const = 0;

	//Depth < 0 means updaing all 2d mips in this layer
	//Depth = FaceIndex for CubeMap
	virtual void UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel = 0, uint16 InArrayIndex = 0, int32 InDepth = -1) = 0;
	virtual void ClearLayerData(uint8 InMipLevel = 0, uint16 InArrayIndex = 0, int32 InDepth = -1) = 0;
	virtual void ClearMipData(uint8 InMipLevel = 0) = 0;
	void ClearAllData(); //Clear all mips and all layers of texture

protected:
	FTextureDesc mDesc;
};

}