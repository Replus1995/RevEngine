#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

struct SceneRenderData
{
	Math::FMatrix4 ProjMatrix;
	Math::FMatrix4 ViewMatrix;
};

class StaticMesh;
struct StaticMeshRenderData
{
	Ref<StaticMesh> StaticMesh;
	Math::FMatrix4 Transform;
};
}