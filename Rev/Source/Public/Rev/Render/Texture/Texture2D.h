#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/PixelFormat.h"

namespace Rev
{
class FRHISampler;
class FRHITexture;
class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	uint16 Width() const;
	uint16 Height() const;
	EPixelFormat Format() const;


private:
	Ref<FRHITexture> mRHITexture;
	Ref<FRHISampler> mRHISampler;
};


}