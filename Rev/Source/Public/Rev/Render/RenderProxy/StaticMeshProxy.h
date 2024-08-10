#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Mesh/StaticMesh.h"

namespace Rev
{
class FScene;
struct FStaticMeshRenderData
{
	Ref<StaticMesh> MeshData;
	FModelUniform UniformData;

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

	void DrawMeshes(EBlendMode InBlend, bool bUseMeshMaterial = true) const;

private:
	void DrawPrimitives(const FStaticMeshRenderData& InData, EBlendMode InBlend, bool bUseMeshMaterial = true) const;

private:
	std::vector<FStaticMeshRenderData> mRenderDataArr;
	TUniform<FModelUniform, UL::BModel> uModel;
};

}