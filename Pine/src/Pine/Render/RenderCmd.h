#pragma once
#include "Pine/Core/Base.h"
#include <glm/glm.hpp>

namespace Pine
{
class BaseRHI;
class VertexArray;
class PINE_API RenderCmd
{
public:
	static void Init();
	static void Shutdown();

	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	static void SetClearColor(const glm::vec4& color);
	static void Clear();

	static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0);
	//static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float lineWidth);

private:
	static Scope<BaseRHI> sRHI;
};
}