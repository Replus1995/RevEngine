#pragma once
#include "Pine/Asset/Asset.h"
#include "Pine/Render/StaticMesh.h"

namespace Pine
{

class PINE_API StaticMeshAsset : public Asset
{
public:
	StaticMeshAsset() = default;
	virtual ~StaticMeshAsset() = default;

	static void SetDefaultMaterial(const Ref<Material>& mat);
	static Ref<StaticMesh> CreateBox();
private:
	static Ref<Material>& GetDefaultMaterial();
};

}