#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

enum class EBasicGeometry : uint8
{
	Box,
	Sphere,
	Capsule,
	Cone
	//...
};

class Material;
class StaticMesh;
class REV_API FAssetLibrary
{
public:
	static const Ref<Material>& GetDefaultMaterial();
	static const Ref<StaticMesh> CreateBasicGeometry(EBasicGeometry InKind, const Ref<Material>& InMaterial = nullptr);

private:
	friend class Application;
	static void Init();
	static void Shutdown();
};

}