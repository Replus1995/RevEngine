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

	static FMeshImportResult ImportMesh(const FPath& InPath);

private:
	friend class Application;
	static void Init();
	static void Shutdown();
};

}