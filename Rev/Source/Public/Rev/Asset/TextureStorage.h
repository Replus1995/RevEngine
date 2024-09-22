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
	FImageStorage(const FImageStorage& Other) = delete;
	FImageStorage(FImageStorage&& Other) noexcept;

	uint8 NumMips();
	uint16 NumLayers();
	void Resize(uint8 NumMips, uint16 NumLayers);
	FBuffer& At(uint8 MipIndex, uint16 LayerIndex);

private:
	std::vector<FBuffer> mImages; //[Size = mNumMips * mNumLayers]
	uint8 mNumMips = 0;
	uint16 mNumLayers = 0;
};

struct FTextureStorage
{
public:
	FTextureStorage() = default;
	~FTextureStorage() = default;
	FTextureStorage(const FTextureStorage& Other) = delete;
	FTextureStorage(FTextureStorage&& Other) noexcept;

	std::string Name;
	FSamplerDesc SamplerDesc;
	FTextureDesc TextureDesc;
	FImageStorage ImageData;

	REV_API Ref<Texture> CreateTexture(bool bForceSRGB = false);
private:
	Ref<Texture> mCache = nullptr;
};

}