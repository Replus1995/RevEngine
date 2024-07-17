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
	void DrawColored(EMaterialDomain InDomain, EBlendMode InBlend);
	void DrawShadow(EMaterialDomain InDomain, EBlendMode InBlend, const Ref<FRHIShaderProgram>& InProgram);

private:
	Ref<StaticMesh> mStaticMesh;
	Math::FMatrix4 mModelMat;
};

}