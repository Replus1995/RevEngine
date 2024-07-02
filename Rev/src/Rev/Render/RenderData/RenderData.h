#include "Pine/Core/Base.h"
#include "Pine/Math/Maths.h"

namespace Pine
{

struct SceneRenderData
{
	FMatrix4 ProjMatrix;
	FMatrix4 ViewMatrix;
};

class StaticMesh;
struct StaticMeshRenderData
{
	Ref<StaticMesh> StaticMesh;
	FMatrix4 Transform;
};
}