#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RenderProxy/UniformRenderProxy.h"

namespace Rev
{
class StaticMesh;
class StaticMeshRenderProxy
{
public:
	StaticMeshRenderProxy();
	~StaticMeshRenderProxy();

	void Prepare(const Ref<StaticMesh>& mesh, const Math::FMatrix4& transform);
	void Draw(EMaterialDomain domain);

private:
	Ref<StaticMesh> mStaticMesh;
	FUniformRenderProxy<Math::FMatrix4, 0> mTransformProxy;
};

}