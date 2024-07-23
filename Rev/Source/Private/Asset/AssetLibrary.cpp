#include "Rev/Asset/AssetLibrary.h"
#include <Rev/Core/FileSystem.h>
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Texture/Texture.h"

#include "./BasicGeometry/PlaneGeometry.hpp"
#include "./BasicGeometry/BoxGeometry.hpp"
#include "./GLTF/GLTFUtils.h"

namespace Rev
{

static Ref<Material> sDefaultMaterial = nullptr;
static Ref<Texture> sDefaultWhiteTexture = nullptr;
static Ref<Texture> sDefaultNormalTexture = nullptr;

void FAssetLibrary::Init()
{
	FFileSystem::MountDir("/Engine", (std::filesystem::current_path() / "Engine").generic_u8string());
}

void FAssetLibrary::Shutdown()
{
	sDefaultWhiteTexture.reset();
	sDefaultNormalTexture.reset();
	sDefaultMaterial.reset();
}

const Ref<Texture>& FAssetLibrary::GetDefaultWhiteTexture()
{
	if (!sDefaultWhiteTexture)
	{
		FTextureDesc TextureDesc = FTextureDesc::MakeTexture2D(2,2, PF_R8G8B8A8, false, Math::FLinearColor(1,1,1,1));
		FSamplerDesc SamplerDesc;
		sDefaultWhiteTexture = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
		sDefaultWhiteTexture->GetResource()->ClearData();
	}
	return sDefaultWhiteTexture;
}

const Ref<Texture>& FAssetLibrary::GetDefaultNormalTexture()
{
	if (!sDefaultNormalTexture)
	{
		FTextureDesc TextureDesc = FTextureDesc::MakeTexture2D(2, 2, PF_RGB8, false, Math::FLinearColor(0, 1, 0));
		FSamplerDesc SamplerDesc;
		sDefaultNormalTexture = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
		sDefaultNormalTexture->GetResource()->ClearData();
	}
	return sDefaultNormalTexture;
}

const Ref<Material>& FAssetLibrary::GetDefaultMaterial()
{
	if (!sDefaultMaterial)
	{
		//FRHIShaderLibrary::GetInstance().LoadOrCompileShader(FPath("/Engine/Shaders/Basic.glsl"));
		auto Program = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("BasicProgram", "/Engine/Shaders/Basic");
		if (Program)
		{
			sDefaultMaterial = CreateRef<Material>();
			sDefaultMaterial->Compile();
		}
	}
	return sDefaultMaterial;
}

Ref<StaticMesh> FAssetLibrary::CreateBasicGeometry(EBasicGeometry InKind, const Ref<Material>& InMaterial)
{
	const Ref<Material>& GeoMat = InMaterial ? InMaterial : GetDefaultMaterial();
	switch (InKind)
	{
	case Rev::EBasicGeometry::Plane:
		return FPlaneGeometry::Create(GeoMat);
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

FMeshImportResult FAssetLibrary::ImportMesh(const FPath& InPath)
{
	if (InPath.Extension() == ".gltf" || InPath.Extension() == ".glb")
	{
		return FGLTFUtils::ImportModel(InPath);
	}

	return FMeshImportResult();
}


}