#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
class FRHIPrimitive;
class FRHIShaderProgram;
class FRHIUniformBuffer;
class FRHITexture;
class FRHIRenderTarget;
class FRHIRenderPass;
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

	static void SetVSync(bool bEnable);
	static void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height);
	static void SetClearColor(const Math::FLinearColor& color);
	static void ClearBackBuffer();

	static void BeginRenderPass(const Ref<FRHIRenderPass>& InRenderPass);
	static void EndRenderPass(bool bBlitToBack = false);
	static void NextSubpass();

	static void BindUniformBuffer(const Ref<FRHIUniformBuffer>& InBuffer, uint16 InBinding);
	static void BindTexture(const Ref<FRHITexture>& InTexture, uint16 InBinding);
	static void BindProgram(const Ref<FRHIShaderProgram>& InProgram);

	static void DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive);

	static FRHIContext* GetContext();
};
}