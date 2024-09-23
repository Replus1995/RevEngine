#pragma once
#include <memory>
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
class FRHIVertexBuffer;
class FRHIIndexBuffer;
class FRHIVertexArray;

class FRHIShaderProgram;
class FRHIUniformBuffer;
class FRHITexture;
class FRHIRenderTarget;

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

	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;
	virtual void SetClearColor(const Math::FLinearColor& color) = 0;
	virtual void ClearBackBuffer() = 0; //to be removed

	virtual void Bind(const Ref<FRHIVertexBuffer>& InVertexBuffer) = 0;
	virtual void Bind(const Ref<FRHIIndexBuffer>& InIndexBuffer) = 0;
	virtual void Bind(const Ref<FRHIVertexArray>& InVertexArray) = 0;

	virtual void Bind(const Ref<FRHIShaderProgram>& InProgram) = 0;
	virtual void Bind(const Ref<FRHIUniformBuffer>& InUniformBuffer, uint32 InUnit) = 0;
	virtual void Bind(const Ref<FRHITexture>& InTexture, uint32 InUnit) = 0;
	virtual void Bind(const Ref<FRHIRenderTarget>& InRenderTarget) = 0;


	virtual void Draw(const Ref<FRHIVertexArray>& InVertexArray) = 0;
};
}