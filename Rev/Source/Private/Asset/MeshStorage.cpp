#include "Rev/Asset/MeshStorage.h"
#include "Rev/Asset/MaterialStorage.h"
#include "Rev/Render/Mesh/StaticMesh.h"

namespace Rev
{

Ref<StaticMesh> FStaticMeshStorage::CreateStaticMesh()
{
	std::vector<Ref<Material>> vMaterials;
	std::vector<FMeshPrimitive> vPrimitives;

	vMaterials.reserve(Materials.size());
	for (auto& Storage : Materials)
	{
		vMaterials.emplace_back(Storage->CreateMaterial());
	}

	vPrimitives.reserve(Primitives.size());
	for (auto& Storage : Primitives)
	{
		vPrimitives.emplace_back(Storage->CreateVertexData());
	}

	Ref<StaticMesh> OutMesh = CreateRef<StaticMesh>();
	OutMesh->SetMaterials(std::move(vMaterials));
	OutMesh->SetPrimitives(std::move(vPrimitives));
	return OutMesh;
}


}
