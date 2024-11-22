#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Component/StaticMesh.h"

namespace Rev
{

struct alignas(16) FStaticMeshUniform
{
	Math::FMatrix4 ModelMat;
};

class FScene;
struct FStaticMeshRenderData
{
	Ref<FStaticMesh> StaticMesh;
	FStaticMeshUniform MeshParams;
	Ref<class FRHIUniformBuffer> MeshUB;
};

class FStaticMeshProxy
{
public:
	FStaticMeshProxy();
	~FStaticMeshProxy();

	void Prepare(const Ref<FScene>& Scene);
	void Cleanup();
	void FreeResource();

	void DrawMeshesDepth(FRHICommandList& RHICmdList);
	void DrawMeshesOpaque(FRHICommandList& RHICmdList, bool bUseMeshMaterial = true);

private:
	void PrepareMeshDraw(FRHICommandList& RHICmdList, const FStaticMeshRenderData& InData);

private:
	std::vector<FStaticMeshRenderData> mRenderDataArr;
	FStaticMeshUniform mModelParams;
};

}