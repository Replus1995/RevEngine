#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{
class FRHIShaderProgram;
class StaticMesh;
class StaticMeshRenderProxy
{
public:
	StaticMeshRenderProxy();
	~StaticMeshRenderProxy();

	void Prepare(const Ref<StaticMesh>& mesh, const Math::FMatrix4& transform);
	void DrawPrimitives(EMaterialDomain InDomain, EBlendMode InBlend, const Ref<FRHIShaderProgram>& InProgram = nullptr);

private:
	Ref<StaticMesh> mStaticMesh;
	Math::FMatrix4 mModelMat;
};

}