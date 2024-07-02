#pragma once
#include "Pine/Render/Mesh.h"

namespace Pine
{

class VertexArray;
struct StaticMeshResource
{
	uint32 MaterialIndex = 0;
	Ref<VertexArray> VertexData;
};

class PINE_API StaticMesh : public Mesh
{
public:
	StaticMesh(const std::vector<Ref<Material>>& pMaterials, const std::vector<StaticMeshResource>& pResources);
	virtual ~StaticMesh();

	virtual const Ref<Material>& GetMaterial(uint32 index) const override;
	virtual uint32 GetMaterialCount() const override;
	virtual std::vector<const VertexArray*> GetVertexArrayByIndex(uint32 index) const override;

private:
	std::vector<Ref<Material>> mMaterials;
	std::vector<StaticMeshResource> mResources;

};

}