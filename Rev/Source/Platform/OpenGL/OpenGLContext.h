#pragma once
#include "Rev/Render/RHI/RHIContext.h"

namespace Rev
{

class OpenGLContext : public FRHIContext
{
public:
	virtual void Init() override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void SetClearColor(const Math::FLinearColor& color) override;
	virtual void Clear() override;

	virtual void DrawIndexed(const Ref<FRHIVertexArray>& vertexArray, uint32_t indexCount) override;
};

}