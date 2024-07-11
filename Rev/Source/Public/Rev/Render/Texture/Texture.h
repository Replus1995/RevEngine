#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/PixelFormat.h"

namespace Rev
{
class FRHITexture;
class Texture
{
public:
	Texture();
	~Texture();

	uint16 Width() const;
	uint16 Height() const;
	EPixelFormat Format() const;

private:
	friend class FTextureStorage;
	Ref<FRHITexture> mRHITexture;
};


}