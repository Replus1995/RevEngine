#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/Texture/Texture.h"
#include <string>

namespace Rev
{

struct FImageImport
{
	EPixelFormat Format;
	uint16 Width;
	uint16 Height;
	FBuffer Data;
};

struct FTextureStorage
{
public:
	std::string Name;
	FSamplerDesc SamplerDesc;
	FTextureDesc TextureDesc;
	FBuffer ImageData;

	Ref<Texture> CreateTexture();
private:
	Ref<Texture> mCache;
};

}