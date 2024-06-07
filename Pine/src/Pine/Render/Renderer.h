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

	static void Submit(const Ref<class Shader>& shader, const Ref<class VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
	//static void Draw(class View* view);
	
	//static void BeginScene(OrthographicCamera& camera);
	//static void EndScene();

	//

	//static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
private:
};
}