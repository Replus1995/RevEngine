#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Asset/TextureStorage.h"
#include "Rev/Asset/MaterialStorage.h"

namespace Rev
{

enum class EBasicGeometry : uint8
{
	Plane,
	Box,
	Sphere,
	Capsule,
	Cone
	//...
};

class FTexture;
class FMaterial;
class FStaticMesh;



struct FModelImportResult
{
	std::vector<Ref<FTextureStorage>> Textures;
	std::vector<Ref<FSurfaceMaterialStorage>> Materials;
	std::vector<Ref<FStaticMesh>> StaticMeshes;
	//SkeletalMeshes
};


class REV_API FAssetLibrary
{
public:
	static const Ref<FMaterial>& GetDefaultSurfaceMaterial();
	static Ref<FStaticMesh> CreateBasicGeometry(EBasicGeometry InKind, const Ref<FMaterial>& InMaterial = nullptr);


	static FTextureStorage ImportTexture(const FPath& InPath);
	static FTextureStorage ImportTextureCube(
		const FPath& InPathPX, const FPath& InPathNX,
		const FPath& InPathPY, const FPath& InPathNY,
		const FPath& InPathPZ, const FPath& InPathNZ
		);

	static FModelImportResult ImportModel(const FPath& InPath);

private:
	friend class Application;
	static void Init();
	static void Shutdown();
};

}