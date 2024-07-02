#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Render/Material.h"

namespace Pine
{
class StaticMesh;
class StaticMeshRenderProxy
{
public:
	StaticMeshRenderProxy() = default;
	~StaticMeshRenderProxy() = default;

	void Prepare(const Ref<StaticMesh>& mesh, const FMatrix4& transform);
	void Draw(EMaterialDomain domain);

private:
	Ref<StaticMesh> mStaticMesh;
	FMatrix4 mTransform;
};

}