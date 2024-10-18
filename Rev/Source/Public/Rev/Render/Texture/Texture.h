#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/PixelFormat.h"

namespace Rev
{
class FRHITexture;
class FRHISamplerState;
class Texture
{
public:
	Texture(const Ref<FRHITexture>& InResource);
	~Texture();

	uint16 Width() const;
	uint16 Height() const;
	uint16 Depth() const;
	uint16 ArraySize() const;
	EPixelFormat Format() const;

	bool IsTexture2D() const;
	bool IsTexture2DArray() const;
	bool IsTextureCube() const;
	bool IsTextureCubeArray() const;
	bool IsTexture3D() const;

	FRHITexture* GetTextureRHI() const { return mTextureRHI.get(); }

private:
	Ref<FRHITexture> mTextureRHI = nullptr;
	Ref<FRHISamplerState> mSamplerStateRHI = nullptr;
};

using Texture2D = Texture;
using Texture2DArray = Texture;
using TextureCube = Texture;
using TextureCubeArray = Texture;
using Texture3D = Texture;

}