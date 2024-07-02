#pragma once
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class ShaderLibrary;
class REV_API Renderer
{
public:
	static void Init();
	static void Shutdown();
	static void OnWindowResize(uint32_t width, uint32_t height);

	static ShaderLibrary* GetShaderLibrary();

	static void Clear(const Math::FVector4& color = Math::FVector4(0,0,0,1));
	static void DrawIndexed(const class VertexArray* vertexArray);
};

}