#pragma once
#include "Rev/Render/Mesh/Mesh.h"

namespace Rev
{

class REV_API StaticMesh : public Mesh
{
public:
	StaticMesh();
	virtual ~StaticMesh();

	void SetMaterials(std::vector<Ref<Material>>&& pMaterials);
	void SetPrimitives(std::vector<FMeshPrimitive>&& pPrimitives);

	virtual const Ref<Material>& GetMaterial(uint32 MaterialIndex) const override;
	virtual uint32 GetMaterialCount() const override;
	virtual std::vector<const FMeshPrimitive*> GetPrimitive(uint32 MaterialIndex) const override;
	virtual uint32 GetPrimitiveCount() const override;

private:
	std::vector<Ref<Material>> mMaterials;
	std::vector<FMeshPrimitive> mPrimitives;

};

}