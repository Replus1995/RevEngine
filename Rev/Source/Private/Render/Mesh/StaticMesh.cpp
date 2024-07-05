#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RHI/RHIVertexBuffer.h"

namespace Rev
{

StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
}

void StaticMesh::SetMaterials(std::vector<Ref<Material>>&& pMaterials)
{
	mMaterials = std::move(pMaterials);
}

void StaticMesh::SetPrimitives(std::vector<FMeshPrimitive>&& pPrimitives)
{
	mPrimitives = std::move(pPrimitives);
}

const Ref<Material>& StaticMesh::GetMaterial(uint32 index) const
{
	if (index < mMaterials.size())
	{
		return mMaterials[index];
	}
	else if (mMaterials.size() > 0)
	{
		return mMaterials[0];
	}
	else
	{
		static Ref<Material> sMeshEmptyMaterial = nullptr;
		return sMeshEmptyMaterial;
	}
}

uint32 StaticMesh::GetMaterialCount() const
{
	return (uint32)mMaterials.size();
}

std::vector<const FMeshPrimitive*> StaticMesh::GetPrimitive(uint32 MaterialIndex) const
{
	std::vector<const FMeshPrimitive*> outArr;
	for (auto& RefPrim : mPrimitives)
	{
		if (RefPrim.MaterialIndex == MaterialIndex)
		{
			outArr.push_back(&RefPrim);
		}
	}
	return outArr;
}

uint32 StaticMesh::GetPrimitiveCount() const
{
	return (uint32)mPrimitives.size();
}


}