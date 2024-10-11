#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{
class FScene;
struct FStaticMeshRenderData
{
	Ref<StaticMesh> MeshData;
	FStaticMeshUniform MeshParams;
	Ref<FRHIUniformBuffer> MeshUB;
};

class FStaticMeshProxy
{
public:
	FStaticMeshProxy();
	~FStaticMeshProxy();

	void Prepare(const Ref<FScene>& Scene);
	void Cleanup();
	void FreeResource();

	void DrawMeshes(EMaterialBlendMode InBlend, bool bUseMeshMaterial = true);

private:
	void DrawPrimitives(const FStaticMeshRenderData& InData, EMaterialBlendMode InBlend, bool bUseMeshMaterial = true) const;

private:
	std::vector<FStaticMeshRenderData> mRenderDataArr;
	FStaticMeshUniform mModelParams;
};

}