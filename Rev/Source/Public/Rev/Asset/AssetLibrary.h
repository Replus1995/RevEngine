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
class StaticMesh;
class REV_API FAssetLibrary
{
public:
	static const Ref<Texture>& GetWhiteTexture();
	static const Ref<Material>& GetDefaultMaterial();
	static Ref<StaticMesh> CreateBasicGeometry(EBasicGeometry InKind, const Ref<Material>& InMaterial = nullptr);

	static FMeshImportResult ImportMesh(const FPath& InPath);

private:
	friend class Application;
	static void Init();
	static void Shutdown();
};

}