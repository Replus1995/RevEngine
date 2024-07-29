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
	Texture(const Ref<FRHITexture>& InResource);
	~Texture();

	uint16 Width() const;
	uint16 Height() const;
	EPixelFormat Format() const;
	const Ref<FRHITexture> GetResource() const { return mResource; }

private:
	Ref<FRHITexture> mResource;
};


}