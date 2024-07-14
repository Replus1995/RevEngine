#include "Rev/Asset/AssetLibrary.h"
#include <Rev/Core/FileSystem.h>
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Mesh/StaticMesh.h"

#include "./BasicGeometry/BoxGeometry.hpp"

namespace Rev
{

static Ref<Material> sDefaultMaterial = nullptr;

void FAssetLibrary::Init()
{
	FFileSystem::MountDir("/Game", std::filesystem::current_path().generic_u8string());
}

void FAssetLibrary::Shutdown()
{
	sDefaultMaterial.reset();
}

const Ref<Material>& FAssetLibrary::GetDefaultMaterial()
{
	if (!sDefaultMaterial)
	{
		FRHIShaderLibrary::GetInstance().LoadOrCompileShader(FPath("/Game/Assets/Shaders/Basic.glsl"));
		auto Program = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("BasicProgram", "Basic");
		if (Program)
			sDefaultMaterial = CreateRef<Material>(Program);
	}
	return sDefaultMaterial;
}

const Ref<StaticMesh> FAssetLibrary::CreateBasicGeometry(EBasicGeometry InKind, const Ref<Material>& InMaterial)
{
	const Ref<Material>& GeoMat = InMaterial ? InMaterial : GetDefaultMaterial();
	switch (InKind)
	{
	case Rev::EBasicGeometry::Box:
		return FBoxGeometry::Create(GeoMat);
	case Rev::EBasicGeometry::Sphere:
		break;
	case Rev::EBasicGeometry::Capsule:
		break;
	case Rev::EBasicGeometry::Cone:
		break;
	default:
		break;
	}

	return nullptr;
}


}