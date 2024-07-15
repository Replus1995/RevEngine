#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Asset/MeshPrimitiveStorage.h"
#include <vector>
#include <string>

namespace Rev
{
struct FTextureStorage;
struct FMaterialStorage;
struct FMeshStorage
{
public:
	std::string Name;
};

struct FStaticMeshStorage : public FMeshStorage
{
public:
	std::vector<Ref<FMaterialStorage>> Materials;
	std::vector<Ref<FMeshPrimitiveStorage>> Primitives;
};

struct FMeshImportResult
{
	std::vector<Ref<FTextureStorage>> Textures;
	std::vector<Ref<FMaterialStorage>> Materials;
	std::vector<Ref<FStaticMeshStorage>> StaticMeshes;
	//SkeletalMeshes
};

}