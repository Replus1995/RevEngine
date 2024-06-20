#include "Pine/Core/Base.h"
#include <glm/glm.hpp>

namespace Pine
{

struct SceneRenderData
{
	glm::mat4 ViewMatrix;
	glm::mat4 ProjMatrix;
};
}