#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Component/StaticMesh.h"

namespace Rev
{
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
	void DrawMeshesOpaque(FRHICommandList& RHICmdList);

private:
	void PrepareMeshDraw(FRHICommandList& RHICmdList, const FStaticMeshRenderData& InData);

private:
	std::vector<FStaticMeshRenderData> mRenderDataArr;
	FStaticMeshUniform mModelParams;
};

}