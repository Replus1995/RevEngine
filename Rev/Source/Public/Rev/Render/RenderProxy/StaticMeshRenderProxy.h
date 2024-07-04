#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{
class StaticMesh;
class StaticMeshRenderProxy
{
public:
	StaticMeshRenderProxy() = default;
	~StaticMeshRenderProxy() = default;

	void Prepare(const Ref<StaticMesh>& mesh, const Math::FMatrix4& transform);
	void Draw(EMaterialDomain domain);

private:
	Ref<StaticMesh> mStaticMesh;
	Math::FMatrix4 mTransform;
};

}