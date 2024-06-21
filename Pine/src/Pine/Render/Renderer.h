#pragma once
#include "Pine/Render/RenderCore.h"
#include "Pine/Math/Math.h"


namespace Pine {
class Renderer
{
public:
	static void Init();
	static void Shutdown();
	static void OnWindowResize(uint32_t width, uint32_t height);
	static void Clear(const glm::vec4& color = glm::vec4(0,0,0,1));

	static void DrawIndexed(const class VertexArray* vertexArray);
};
}