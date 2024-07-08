#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Math/Maths.h"
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
};

struct FRHITextureDesc
{
	Math::FLinearColor ClearColor;
	uint16 Width = 1;
	uint16 Height = 1;
	uint16 Depth = 1; //For 3D Texture
	uint16 ArraySize = 1; //For Texture Array
	ETextureDimension Dimension = ETextureDimension::Texture2D;
	EPixelFormat Format = PF_R8G8B8A8;
	uint8 NumMips = 1;
	uint8 NumSamples = 1; //For MSAA
};

class FRHITexture
{
public:
	virtual ~FRHITexture() = default;
	const FRHITextureDesc& GetDesc() const { return mDesc; }
	uint32 GetWidth() const { return mDesc.Width; };
	uint32 GetHeight() const { return mDesc.Height; };

	virtual void SetData(const void* InData, uint32 InSize) = 0;
	virtual void Bind(uint32 InSlot = 0) const = 0;
protected:
	FRHITexture(const FRHITextureDesc& InDesc)
		: mDesc(InDesc)
	{}

protected:
	FRHITextureDesc mDesc;
};

}