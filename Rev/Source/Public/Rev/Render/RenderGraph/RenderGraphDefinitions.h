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

enum class ERGTextureFlags : uint8
{
	None = 0,
	MultiFrame = 1 << 0,
};
ENUM_CLASS_FLAGS(ERGTextureFlags)

enum class ERGBufferFlags : uint8
{
	None = 0,
	MultiFrame = 1 << 0,
};
ENUM_CLASS_FLAGS(ERGBufferFlags)

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
	static FRGBufferDesc CreateBuffer(uint32 ElementSize, uint32 NumElements)
	{
		FRGBufferDesc Desc;
		Desc.Usage = EBufferUsageFlags::Static | EBufferUsageFlags::ShaderResource | EBufferUsageFlags::VertexBuffer;
		Desc.ElementSize = ElementSize;
		Desc.NumElements = NumElements;
		return Desc;
	}

	uint32 GetSize() const
	{
		return ElementSize * NumElements;
	}

	bool operator == (const FRGBufferDesc& Other) const
	{
		bool bSame = ElementSize == Other.ElementSize &&
			NumElements == Other.NumElements &&
			Usage == Other.Usage;
		return bSame;
	}

	bool operator != (const FRGBufferDesc& Other) const
	{
		return !(*this == Other);
	}


	uint32 ElementSize = 1;
	uint32 NumElements = 1;
	EBufferUsageFlags Usage = EBufferUsageFlags::None;
};

class FRGResource;
using FRGResourceRef = FRGResource*;

class FRGTexture;
using FRGTextureRef = FRGTexture*;

class FRGBuffer;
using FRGBufferRef = FRGBuffer*;

class FRGUniformBuffer;
using FRGUniformBufferRef = FRGUniformBuffer*;

class FRGPass;
using FRGPassRef = FRGPass*;




}