#include "Pine/Core/Base.h"
#include <glm/glm.hpp>

namespace Pine
{


struct SceneRenderData
{
	glm::mat4 ProjMatrix;
	glm::mat4 ViewMatrix;
};

class StaticMesh;
struct StaticMeshRenderData
{
	Ref<StaticMesh> StaticMesh;
	glm::mat4 Transform;
};
}