#pragma once
#include "Rev/Asset/Mesh/MeshPrimitiveStorage.h"
#include <vector>

namespace Rev
{

struct FStaticMeshStorage
{
public:
	//std::vector<Ref<class FMaterialStorage>> mMaterialData;
	std::vector<Ref<FMeshPrimitiveStorage>> mPrimitiveData;
};

}