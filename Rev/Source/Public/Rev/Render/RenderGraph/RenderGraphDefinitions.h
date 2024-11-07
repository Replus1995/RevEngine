#pragma once
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIBuffer.h"

namespace Rev
{

enum class ERGViewableResourceType : uint8
{
	Texture,
	Buffer,
	Count
};

struct FRGTextureDesc : public FRHITextureDesc
{
	static FRGTextureDesc Create2D(
		uint16 Width,
		uint16 Height,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint8 NumMips = 1,
		uint8 NumSamples = 1
	)
	{
		const uint16 Depth = 1;
		const uint16 ArraySize = 1;
		return FRGTextureDesc(ETextureDimension::Texture2D, Flags, Format, ClearColor, Width, Height, Depth, ArraySize, NumMips, NumSamples);
	}

	static FRGTextureDesc Create2DArray(
		uint16 Width,
		uint16 Height,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint16 ArraySize,
		uint8 NumMips = 1,
		uint8 NumSamples = 1
	)
	{
		const uint16 Depth = 1;
		return FRGTextureDesc(ETextureDimension::Texture2DArray, Flags, Format, ClearColor, Width, Height, Depth, ArraySize, NumMips, NumSamples);
	}

	static FRGTextureDesc Create3D(
		uint16 Width,
		uint16 Height,
		uint16 Depth,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint8 NumMips = 1
	)
	{
		const uint16 ArraySize = 1;
		const uint8 LocalNumSamples = 1;
		return FRGTextureDesc(ETextureDimension::Texture3D, Flags, Format, ClearColor, Width, Height, Depth, ArraySize, NumMips, LocalNumSamples);
	}

	static FRGTextureDesc CreateCube(
		uint32 Size,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint8 NumMips = 1,
		uint8 NumSamples = 1
	)
	{
		const uint16 Depth = 1;
		const uint16 ArraySize = 1;
		return FRGTextureDesc(ETextureDimension::TextureCube, Flags, Format, ClearColor, Size, Size, Depth, ArraySize, NumMips, NumSamples);
	}

	static FRGTextureDesc CreateCubeArray(
		uint32 Size,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint16 ArraySize,
		uint8 NumMips = 1,
		uint8 NumSamples = 1
	)
	{
		const uint16 Depth = 1;
		return FRGTextureDesc(ETextureDimension::TextureCubeArray, Flags, Format, ClearColor, Size, Size, Depth, ArraySize, NumMips, NumSamples);
	}

	FRGTextureDesc() = default;
	FRGTextureDesc(
		ETextureDimension InDimension,
		ETextureCreateFlags InFlags,
		EPixelFormat InFormat,
		const FRHITextureClearColor& InClearValue,
		uint16 InWidth,
		uint16 InHeight,
		uint16 InDepth,
		uint16 InArraySize,
		uint8 InNumMips,
		uint8 InNumSamples
	)
		: FRHITextureDesc(InDimension, InFlags, InFormat, InClearValue, InWidth, InHeight, InDepth, InArraySize, InNumMips, InNumSamples)
	{
	}

};


struct FRGBufferDesc
{

};

}