#pragma once
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class REV_API RenderUtils
{
public:
	static void Init();
	static void Shutdown();
	static void DrawScreenQuad(); //For PostProcess
};

}