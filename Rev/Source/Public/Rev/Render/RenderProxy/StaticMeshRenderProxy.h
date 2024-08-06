#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class FRHIShaderProgram;
class StaticMesh;
class StaticMeshRenderProxy
{
public:
	StaticMeshRenderProxy();
	~StaticMeshRenderProxy();

	void Prepare(const Ref<StaticMesh>& InMesh, const Math::FTransform& InTransform);
	void DrawPrimitives(EBlendMode InBlend, const Ref<FRHIShaderProgram>& InProgram = nullptr) const;
	const Math::FMatrix4& GetMatrix() const { return mModelMat; }
private:
	Ref<StaticMesh> mStaticMesh;
	Math::FMatrix4 mModelMat;
};

}