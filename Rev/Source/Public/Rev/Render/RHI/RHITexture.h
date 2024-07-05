#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/PixelFormat.h"
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

struct TextureDescription
{
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

	virtual const TextureDescription& GetDesc() const = 0;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual uint32_t GetHandle() const = 0;

	virtual void SetData(void* data, uint32_t size) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;


	virtual bool operator==(const FRHITexture& other) const = 0;
};

class Texture2D : public FRHITexture
{
public:
	static Ref<Texture2D>  (const TextureDescription& desc);
};

}