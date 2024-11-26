#pragma once
#include "Rev/Core/Base.h"
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
	std::vector<Ref<FMaterial>> Materials;
	std::vector<Ref<FStaticMesh>> StaticMeshes;
	//SkeletalMeshes
};


class REV_API FAssetLibrary
{
public:
	static const Ref<FMaterial>& GetDefaultSurfaceMaterial();
	static Ref<FStaticMesh> CreateBasicGeometry(EBasicGeometry InKind, const Ref<FMaterial>& InMaterial = nullptr);


	static FTextureStorage ImportTexture(const char* InPath);
	static FTextureStorage ImportTextureCube(
		const char* InPathPX, const char* InPathNX,
		const char* InPathPY, const char* InPathNY,
		const char* InPathPZ, const char* InPathNZ,
		bool bSRGB = false
		);

	static FModelImportResult ImportModel(const char* InPath);

private:
	friend class Application;
	static void Init();
	static void Shutdown();
};

}