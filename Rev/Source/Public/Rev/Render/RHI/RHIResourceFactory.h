#pragma once
#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

class REV_API FRHIResourceFactory
{
public:
	static Ref<FRHITexture> CreateTexture(const FRHITextureDesc& InDesc);

};

}