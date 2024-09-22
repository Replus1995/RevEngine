#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Asset/MeshStorage.h"

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

class Texture;
class Material;
class SurfaceMaterial;
class StaticMesh;
class REV_API FAssetLibrary
{
public:
	static const Ref<Texture>& GetDefaultWhiteTexture2D();
	static const Ref<Texture>& GetDefaultBlackTexture2D();
	static const Ref<Texture>& GetDefaultNormalTexture2D();
	static const Ref<SurfaceMaterial>& GetDefaultSurfaceMaterial();
	static Ref<StaticMesh> CreateBasicGeometry(EBasicGeometry InKind, const Ref<SurfaceMaterial>& InMaterial = nullptr);


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