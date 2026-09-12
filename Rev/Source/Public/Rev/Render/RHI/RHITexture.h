#pragma once
#include "Rev/Math/Maths.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"
#include <algorithm>

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

struct FRHITextureDesc
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

	FRHITextureDesc() {}

	FRHITextureDesc(ETextureDimension InDimension, EPixelFormat InFormat)
		: Dimension(InDimension)
		, Format(InFormat)
	{
	}

	FRHITextureDesc(
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
	}

	FRHITextureDesc& operator=(const FRHITextureDesc& Other)
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

	FRHITextureDesc& SetFlags(ETextureCreateFlags InFlags) { Flags = InFlags; return *this; }
	FRHITextureDesc& AddFlags(ETextureCreateFlags InFlags) { Flags |= InFlags; return *this; }
	FRHITextureDesc& SetExtent(uint16 InWidth, uint16 InHeight, uint16 InDepth = 1) { Width = InWidth; Height = InHeight; Depth = InDepth; return *this; }
	FRHITextureDesc& SetArraySize(uint16 InArraySize) { ArraySize = InArraySize; return *this; }
	FRHITextureDesc& SetClearColor(const FRHITextureClearColor& InClearColor) { ClearColor = InClearColor; return *this; }
	FRHITextureDesc& SetNumMips(uint8 InNumMips) { NumMips = InNumMips; return *this; }
	FRHITextureDesc& SetNumSamples(uint8 InNumSamples) { NumSamples = InNumSamples; return *this; }

	static uint8 CalculateFullMipCount(uint32 InWidth, uint32 InHeight, uint32 InDepth = 1)
	{
		uint32 MaxDimension = std::max({ InWidth, InHeight, InDepth });
		uint8 Result = 1;
		while (MaxDimension > 1) { MaxDimension >>= 1; ++Result; }
		return Result;
	}

	uint16 GetPhysicalLayerCount() const
	{
		switch (Dimension)
		{
		case ETextureDimension::Texture2DArray: return ArraySize;
		case ETextureDimension::TextureCube: return 6;
		case ETextureDimension::TextureCubeArray: return uint16(6 * ArraySize);
		default: return 1;
		}
	}

	Math::FVector3 GetMipExtent(uint8 InMipLevel) const
	{
		return Math::FVector3(
			float(std::max<uint32>(1, Width >> InMipLevel)),
			float(std::max<uint32>(1, Height >> InMipLevel)),
			float(std::max<uint32>(1, Depth >> InMipLevel)));
	}

	bool Validate() const
	{
		if (Width == 0 || Height == 0 || Depth == 0 || NumMips == 0 || Format == PF_Unknown) return false;
		if (NumMips > CalculateFullMipCount(Width, Height, Depth)) return false;
		if (Dimension != ETextureDimension::Texture3D && Depth != 1) return false;
		if ((Dimension == ETextureDimension::TextureCube || Dimension == ETextureDimension::TextureCubeArray) && Width != Height) return false;
		if ((Dimension == ETextureDimension::Texture2DArray || Dimension == ETextureDimension::TextureCubeArray) && ArraySize == 0) return false;
		if (Dimension == ETextureDimension::Texture3D && ArraySize != 1) return false;
		if ((Dimension == ETextureDimension::Texture2D || Dimension == ETextureDimension::TextureCube) && ArraySize != 1) return false;
		if (Dimension != ETextureDimension::Texture2D && NumSamples != 1) return false;
		if (NumSamples > 1 && NumMips != 1) return false;
		return true;
	}


	static FRHITextureDesc Create2D(uint16 InWidth, uint16 InHeight, EPixelFormat InFormat)
	{
		FRHITextureDesc Desc(ETextureDimension::Texture2D, InFormat);
		return Desc.SetExtent(InWidth, InHeight);
	}
	
	static FRHITextureDesc Create2DArray(uint16 InWidth, uint16 InHeight, uint16 InArraySize, EPixelFormat InFormat)
	{
		FRHITextureDesc Desc(ETextureDimension::Texture2DArray, InFormat);
		return Desc.SetExtent(InWidth, InHeight).SetArraySize(InArraySize);
	}

	static FRHITextureDesc CreateCube(uint16 InSize, EPixelFormat InFormat)
	{
		FRHITextureDesc Desc(ETextureDimension::TextureCube, InFormat);
		return Desc.SetExtent(InSize, InSize);
	}

	static FRHITextureDesc CreateCubeArray(uint16 InSize, uint16 InArraySize, EPixelFormat InFormat)
	{
		FRHITextureDesc Desc(ETextureDimension::TextureCubeArray, InFormat);
		return Desc.SetExtent(InSize, InSize).SetArraySize(InArraySize);
	}

	static FRHITextureDesc Create3D(uint16 InWidth, uint16 InHeight, uint16 InDepth, EPixelFormat InFormat)
	{
		FRHITextureDesc Desc(ETextureDimension::Texture3D, InFormat);
		return Desc.SetExtent(InWidth, InHeight, InDepth);
	}

};

struct FRHITextureSubresource
{
	uint16 MipLevel = 0;
	uint16 ArrayLayer = 0;
};

struct FRHITextureRegion
{
	uint32 X = 0;
	uint32 Y = 0;
	uint32 Z = 0;
	uint32 Width = 0;
	uint32 Height = 0;
	uint32 Depth = 0;
};

struct FRHITextureUpdateDesc
{
	FRHITextureSubresource Subresource;
	FRHITextureRegion Region;
	const void* Data = nullptr;
	uint64 DataSize = 0;
	uint32 RowPitch = 0;
	uint32 SlicePitch = 0;
};

struct FRHITextureCopyDesc
{
	FRHITextureSubresource SourceSubresource;
	FRHITextureSubresource DestinationSubresource;
	FRHITextureRegion SourceRegion;
	FRHITextureRegion DestinationRegion;
	uint16 LayerCount = 1;
};

enum class ERHITextureViewType : uint8
{
	Auto,
	ShaderResource,
	Attachment,
};

struct FRHITextureViewDesc
{
	FRHITextureSubresourceRange Range;
	ERHITextureViewType Type = ERHITextureViewType::Auto;
};

class REV_API FRHITexture : public FRHIResource
{
public:
	FRHITexture(const FRHITextureDesc& InDesc) : TextureDesc(InDesc) {}
	virtual ~FRHITexture() = default;

	const FRHITextureDesc& GetDesc() const { return TextureDesc; }
	uint16 GetWidth() const { return TextureDesc.Width; }
	uint16 GetHeight() const { return TextureDesc.Height; }
	uint16 GetDepth() const { return TextureDesc.Depth; }
	uint16 GetArraySize() const { return TextureDesc.ArraySize; }
	EPixelFormat GetFormat() const { return TextureDesc.Format; }
	FPixelFormatInfo GetFormatInfo() const { return GPixelFormats[TextureDesc.Format]; }

protected:
	FRHITextureDesc TextureDesc;
};

}
