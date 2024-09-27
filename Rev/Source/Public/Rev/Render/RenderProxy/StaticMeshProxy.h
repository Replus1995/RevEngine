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
	FStaticMeshUniform ModelParams;
	uint32 DynamicOffset;
};

class FStaticMeshProxy
{
public:
	FStaticMeshProxy();
	~FStaticMeshProxy();

	void Prepare(const Ref<FScene>& Scene, FRenderer* Renderer);
	void Cleanup();
	void FreeResource();

	void DrawMeshes(const FRenderer* Renderer, EMaterialBlendMode InBlend, bool bUseMeshMaterial = true);

private:
	void DrawPrimitives(const FStaticMeshRenderData& InData, const FRenderer* Renderer, EMaterialBlendMode InBlend, bool bUseMeshMaterial = true) const;

private:
	std::vector<FStaticMeshRenderData> mRenderDataArr;
	FStaticMeshUniform mModelParams;
};

}