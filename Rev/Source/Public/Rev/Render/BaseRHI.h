#pragma once
#include <memory>
#include "Rev/Math/Maths.h"

namespace Rev
{
class VertexArray;
class BaseRHI
{
public:
	virtual ~BaseRHI() = default;

	virtual void Init() = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void SetClearColor(const Math::FLinearColor& color) = 0;
	virtual void Clear() = 0;

	virtual void DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount = 0) = 0;
	//virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float width) = 0;
	//virtual void DrawPoints(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float d) = 0;

	static std::unique_ptr<BaseRHI> Create();
};
}