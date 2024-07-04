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

	static void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height);
	static void SetClearColor(const Math::FLinearColor& color);
	static void Clear();

	static void DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount = 0);
	//static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float lineWidth);

private:
	static Scope<BaseRHI> sRHI;
};
}