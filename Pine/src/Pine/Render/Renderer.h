#pragma once
#include "Pine/Render/RenderCore.h"
#include "Pine/Math/Math.h"


namespace Pine
{
class ShaderLibrary;
class PINE_API Renderer
{
public:
	static void Init();
	static void Shutdown();
	static void OnWindowResize(uint32_t width, uint32_t height);

	static ShaderLibrary* GetShaderLibrary();

	static void Clear(const FVector4& color = FVector4(0,0,0,1));
	static void DrawIndexed(const class VertexArray* vertexArray);
};

}