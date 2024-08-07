#pragma once
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class REV_API Renderer
{
public:
	static void Init();
	static void Shutdown();
	static void ResizeWindow(uint32 InWidth, uint32 InHeight);
	static void DrawScreenQuad(); //For PostProcess
};

}