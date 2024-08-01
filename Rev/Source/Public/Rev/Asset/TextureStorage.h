#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/Texture/Texture.h"
#include <vector>

namespace Rev
{

struct FImageStorage
{
public:
	FImageStorage() = default;
	~FImageStorage() = default;

	uint8 NumMips();
	uint8 NumLayers();
	void Resize(uint8 NumMips, uint8 NumLayers);
	FBuffer& At(uint8 MipIndex, uint8 LayerIndex);

private:
	std::vector<std::vector<FBuffer>> mImages; //[size = TextureDesc.ArraySize][size = TextureDesc.MipNum]
};

struct FTextureStorage
{
public:
	std::string Name;
	FSamplerDesc SamplerDesc;
	FTextureDesc TextureDesc;
	FImageStorage ImageData;

	Ref<Texture> CreateTexture(bool bForceSRGB = false);
private:
	Ref<Texture> mCache = nullptr;
};

}