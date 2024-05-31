#pragma once
#include <memory>
#include <glm/glm.hpp>

namespace Pine
{
class BaseRHI
{
public:
	virtual ~BaseRHI() = default;

	virtual void Init() = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void SetClearColor(const glm::vec4& color) = 0;
	virtual void Clear() = 0;

	//virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
	//virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
	//virtual void SetLineWidth(float width) = 0;

	static std::unique_ptr<BaseRHI> Create();
};
}