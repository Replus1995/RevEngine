#pragma once
#include "Pine/Core/Base.h"

namespace Pine
{
class StaticMesh;
class SceneRenderProxy;

class StaticMeshRenderProxy
{
public:
	StaticMeshRenderProxy() = default;
	~StaticMeshRenderProxy() = default;

	void Prepare(const Ref<StaticMesh>& mesh);
	void Draw(SceneRenderProxy* pSceneProxy);
};

}