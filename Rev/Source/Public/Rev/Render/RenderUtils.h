#pragma once
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

extern REV_API class FTexture* GWhiteTexture;
extern REV_API class FTexture* GBlackTexture;
extern REV_API class FTexture* GNormalTexture;

extern REV_API Ref<class FRHIPrimitive> GScreenPlanePrimitive;

class REV_API RenderUtils
{
public:
	static void Init();
	static void Shutdown();
};

}