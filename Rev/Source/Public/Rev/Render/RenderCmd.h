#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
struct FMeshPrimitive;
class FRHIVertexArray;

class FRHIShaderProgram;
class FRHIUniformBuffer;
class FRHITexture;
class FRHIRenderTarget;
class FRHIContext;
class REV_API RenderCmd
{
public:
	static void Init();
	static void Shutdown();

	static void BeginFrame();
	static void EndFrame();

	static void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height);
	static void SetClearColor(const Math::FLinearColor& color);
	static void ClearBackBuffer();

	static void EnableDepthTest(bool bEnable);
	static void EnableDepthWrite(bool bEnable);
	static void SetDepthTestMode(EDepthTestMode InMode);
	static void SetBlendMode(EBlendMode InMode);
	static void SetCullFaceMode(ECullFaceMode InMode);

	static void BindProgram(const Ref<FRHIShaderProgram>& InProgram);
	static void BindUniformBuffer(const Ref<FRHIUniformBuffer>& InUniformBuffer, uint32 InUnit);
	static void BindTexture(const Ref<FRHITexture>& InTexture, uint32 InUnit);
	static void BindRenderTarget(const Ref<FRHIRenderTarget>& InRenderTarget);

	static void Draw(const Ref<FRHIVertexArray>& InVertexArray, EDrawMode InDrawMode);
	static void DrawPrimitive(const FMeshPrimitive* InPrimitive);
	//static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float lineWidth);

	static FRHIContext* GetContext();
};
}