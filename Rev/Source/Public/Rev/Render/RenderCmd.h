#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
class FRHIPrimitive;
class FRHIShaderProgram;
class FRHIUniformBuffer;
class FRHIUniformBufferDynamic;
class FRHITexture;
class FRHIRenderTarget;
class FRHIContext;

class REV_API RenderCmd
{
public:
	static void Init();
	static void Shutdown();
	static void Flush();

	static void BeginFrame(bool bClearBackBuffer = true);
	static void EndFrame();
	static void PresentFrame();

	static void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height);
	static void SetClearColor(const Math::FLinearColor& color);
	static void ClearBackBuffer();

	static void DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive, const Ref<FRHIShaderProgram>& InProgram, const Ref<FRHIUniformBufferDynamic>& InUniformBufferDynamic, uint32 InDynamicOffset);

	static FRHIContext* GetContext();
};
}