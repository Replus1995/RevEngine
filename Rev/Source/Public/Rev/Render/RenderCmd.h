#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
class Material;
struct FMeshPrimitive;
class FRHIVertexArray;
class REV_API RenderCmd
{
public:
	static void Init();
	static void Shutdown();

	static void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height);
	static void SetClearColor(const Math::FLinearColor& color);
	static void Clear();

	static void PrepareMaterial(const Material* InMaterial);
	static void DrawPrimitive(const FMeshPrimitive* InPrimitive);
	static void DrawVertices(const Ref<FRHIVertexArray>& InVertexArray, EDrawMode InDrawMode);
	//static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float lineWidth);
};
}