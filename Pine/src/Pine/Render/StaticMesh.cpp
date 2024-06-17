#include "pinepch.h"
#include "StaticMesh.h"
#include "Pine/Render/Material.h"
#include "Pine/Render/Resource/VertexBuffer.h"

namespace Pine
{

StaticMesh::StaticMesh(const std::vector<Ref<Material>>& pMaterials, const std::vector<StaticMeshResource>& pResources)
	: mMaterials(pMaterials)
	, mResources(pResources)
{
}

StaticMesh::~StaticMesh()
{
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
	return mMaterials.size();
}

std::vector<const Ref<VertexArray>&> StaticMesh::GetVertexArrayByIndex(uint32 index) const
{
	std::vector<const Ref<VertexArray>&> outArr;
	for (auto res : mResources)
	{
		if (res.MaterialIndex == index)
		{
			outArr.push_back(res.VertexData);
		}
	}
	return outArr;
}

}