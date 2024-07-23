#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Asset/MeshPrimitiveStorage.h"
#include <vector>
#include <string>

namespace Rev
{
class StaticMesh;
struct FTextureStorage;
struct FMaterialStorage;
struct REV_API FMeshStorage
{
public:
	std::string Name;
};

struct REV_API FStaticMeshStorage : public FMeshStorage
{
public:
	std::vector<Ref<FMaterialStorage>> Materials;
	std::vector<Ref<FMeshPrimitiveStorage>> Primitives;

	Ref<StaticMesh> CreateStaticMesh();
private:
	Ref<StaticMesh> mCache = nullptr;
};

struct FMeshImportResult
{
	std::vector<Ref<FTextureStorage>> Textures;
	std::vector<Ref<FMaterialStorage>> Materials;
	std::vector<Ref<FStaticMeshStorage>> StaticMeshes;
	//SkeletalMeshes
};

}