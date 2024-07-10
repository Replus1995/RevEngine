#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Asset/MeshPrimitiveStorage.h"
#include <vector>
#include <string>

namespace Rev
{

struct FStaticMeshStorage
{
public:
	std::string Name;
	std::vector<Ref<FMeshPrimitiveStorage>> PrimitiveData;
};

}