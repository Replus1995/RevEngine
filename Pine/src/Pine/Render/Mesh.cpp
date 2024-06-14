#include "pinepch.h"
#include "Mesh.h"

namespace Pine
{

Mesh::~Mesh()
{
}

Mesh::Mesh()
{
}

Mesh::Mesh(const std::vector<Ref<Material>>& pMaterials, const std::vector<Ref<UnitMesh>>& pUnitMeshes)
	: mMaterials(pMaterials)
	, mUnitMeshes(pUnitMeshes)
{
}

Material* Mesh::GetMaterial(uint32_t index) const
{
	if (index < mMaterials.size())
	{
		return mMaterials[index].get();
	}
	else if (mMaterials.size() > 0)
	{
		return mMaterials[0].get();
	}
	return nullptr;
}

StaticMesh::StaticMesh(const std::vector<Ref<Material>>& pMaterials, const std::vector<Ref<UnitMesh>>& pUnitMeshes)
	: Mesh(pMaterials, pUnitMeshes)
{

}

StaticMesh::~StaticMesh()
{
}

}