#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/Mesh/MeshPrimitive.h"
#include <glad/gl.h>

namespace Rev
{

class FOpenGLContext : public FRHIContext
{
public:
	virtual void Init() override;
	virtual void Cleanup() override;
	virtual void Flush() override {};

	virtual void BeginFrame(bool bClearBackBuffer) override {};
	virtual void EndFrame() override {};
	virtual void PresentFrame() override {};

	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) override;
	virtual void SetClearColor(const Math::FLinearColor& color) override;
	virtual void ClearBackBuffer() override;

	virtual void DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive, const Ref<FRHIShaderProgram>& InProgram, const Ref<FRHIUniformBufferDynamic>& InUniformBufferDynamic, uint32 InDynamicOffset) override;

private:
	//GLenum TranslateDrawMode(EDrawMode InDrawMode);
	GLenum TranslateIndexType(uint32 InStride);
};

}