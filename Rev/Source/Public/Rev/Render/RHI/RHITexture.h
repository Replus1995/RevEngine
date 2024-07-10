#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIResource.h"
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

struct FTextureDesc
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

class REV_API FRHITexture : public FRHIResource
{
public:
	virtual ~FRHITexture() = default;
	const FTextureDesc& GetDesc() const { return mDesc; }
	uint32 GetWidth() const { return mDesc.Width; };
	uint32 GetHeight() const { return mDesc.Height; };

	virtual void UpdateData(const void* InData, uint32 InSize) = 0;
	virtual void Bind(uint32 InUnit) const = 0;
protected:
	FRHITexture(const FTextureDesc& InDesc) : mDesc(InDesc) {}
protected:
	FTextureDesc mDesc;
};

}