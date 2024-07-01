#pragma once
#include "Pine/Render/BaseRHI.h"

namespace Pine
{

class OpenGLRHI : public BaseRHI
{
public:
	virtual void Init() override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void SetClearColor(const FVector4& color) override;
	virtual void Clear() override;

	virtual void DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount) override;
};

}