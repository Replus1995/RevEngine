#pragma once
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class FRHIShaderLibrary;
class REV_API Renderer
{
public:
	static void Init();
	static void Shutdown();
	static void ResizeWindow(uint32 InWidth, uint32 InHeight);
	static FRHIShaderLibrary* GetShaderLibrary();
	static Math::FLinearColor sClearColor;
};

}