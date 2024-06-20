#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Render/Material.h"
#include <glm/glm.hpp>

namespace Pine
{
class StaticMesh;
class StaticMeshRenderProxy
{
public:
	StaticMeshRenderProxy() = default;
	~StaticMeshRenderProxy() = default;

	void Prepare(const Ref<StaticMesh>& mesh, const glm::mat4& transform);
	void Draw(EMaterialDomain domain);

private:
	Ref<StaticMesh> mStaticMesh;
	glm::mat4 mTransform;
};

}