#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class BaseRHI;
class VertexArray;
class REV_API RenderCmd
{
public:
	static void Init();
	static void Shutdown();

	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	static void SetClearColor(const Math::FVector4& color);
	static void Clear();

	static void DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount = 0);
	//static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float lineWidth);

private:
	static Scope<BaseRHI> sRHI;
};
}