#pragma once
#include "Pine/Render/Resource/VertexBuffer.h"
#include <vector>

namespace Pine
{

struct UnitMesh
{
	uint32_t mMaterialIndex = 0;
	Ref<VertexArray> mVertexData = nullptr;
};

class Material;
class PINE_API Mesh
{
public:
	virtual ~Mesh();

	Material* GetMaterial(uint32_t index) const;
	const std::vector<Ref<Material>>& GetMaterials() const
	{
		return mMaterials;
	}
	const std::vector<Ref<UnitMesh>> GetUnitMeshes() const
	{
		return mUnitMeshes;
	}

protected:
	Mesh();
	Mesh(const std::vector<Ref<Material>>& pMaterials, const std::vector<Ref<UnitMesh>>& pUnitMeshes);

protected:
	std::vector<Ref<Material>> mMaterials;
	std::vector<Ref<UnitMesh>> mUnitMeshes;
};

//mMaterials & mUnitMeshes should be the same size in StaticMesh
class PINE_API StaticMesh : public Mesh
{
public:
	StaticMesh(const std::vector<Ref<Material>>& pMaterials, const std::vector<Ref<UnitMesh>>& pUnitMeshes);
	virtual ~StaticMesh();

};

}