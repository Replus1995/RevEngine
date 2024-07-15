#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class Material;
class MeshPrimitive;
class REV_API RenderCmd
{
public:
	static void Init();
	static void Shutdown();

	static void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height);
	static void SetClearColor(const Math::FLinearColor& color);
	static void Clear();

	static void PrepareMaterial(const Material* InMaterial);
	static void DrawPrimitive(const MeshPrimitive* InPrimitive);
	//static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float lineWidth);
};
}