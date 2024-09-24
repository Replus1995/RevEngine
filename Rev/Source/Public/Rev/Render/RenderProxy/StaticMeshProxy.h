#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{
class FRenderer;
class FScene;
struct FStaticMeshRenderData
{
	Ref<StaticMesh> MeshData;
	FModelUniform ModelParams;

	FStaticMeshRenderData() = default;
	FStaticMeshRenderData(const Ref<StaticMesh>& InStaticMesh, const Math::FMatrix4& InModelMatrix);
};

class FStaticMeshProxy
{
public:
	FStaticMeshProxy();
	~FStaticMeshProxy();

	void Prepare(const Ref<FScene>& Scene);
	void Cleanup();
	void FreeResource();

	void DrawMeshes(const FRenderer* Renderer, const Math::FMatrix4& ViewProjMat, EMaterialBlendMode InBlend, bool bUseMeshMaterial = true);

private:
	void DrawPrimitives(const FStaticMeshRenderData& InData, EMaterialBlendMode InBlend, bool bUseMeshMaterial = true) const;

private:
	std::vector<FStaticMeshRenderData> mRenderDataArr;
	FModelUniform mModelParams;
};

}