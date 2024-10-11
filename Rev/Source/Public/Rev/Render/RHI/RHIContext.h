#pragma once
#include <memory>
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
class FRHIVertexBuffer;
class FRHIIndexBuffer;
class FRHIPrimitive;
class FRHIShaderProgram;
class FRHIUniformBuffer;
class FRHITexture;
class FRHIRenderTarget;
class FRHIRenderPass;

class FRHIContext
{
public:
	virtual ~FRHIContext() = default;

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual void Flush() = 0;

	virtual void BeginFrame(bool bClearBackBuffer) = 0;
	virtual void EndFrame() = 0;
	virtual void PresentFrame() = 0;

	//virtual void BeginCompute() = 0;
	//virtual void EndCompute() = 0;

	virtual void SetVSync(bool bEnable) = 0;
	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;
	virtual void SetClearColor(const Math::FLinearColor& color) = 0;
	virtual void ClearBackBuffer() = 0; //to be removed

	virtual void BeginRenderPass(const Ref<FRHIRenderPass>& InRenderPass) = 0;
	virtual void EndRenderPass() = 0;
	virtual void NextSubpass() = 0;


	virtual void BindUniformBuffer(const Ref<FRHIUniformBuffer>& InBuffer, uint16 InBinding) = 0;
	virtual void BindTexture(const Ref<FRHITexture>& InTexture, uint16 InBinding) = 0;
	virtual void BindProgram(const Ref<FRHIShaderProgram>& InProgram) = 0;

	virtual void DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive) = 0;
};
}