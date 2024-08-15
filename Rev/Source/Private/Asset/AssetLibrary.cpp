#include "Rev/Asset/AssetLibrary.h"
#include <Rev/Core/FileSystem.h>
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Texture/Texture.h"

#include "./BasicGeometry/PlaneGeometry.hpp"
#include "./BasicGeometry/BoxGeometry.hpp"
#include "./GLTF/GLTFUtils.h"
#include "./STB/STBImage.h"

namespace Rev
{

static Ref<SurfaceMaterial> sDefaultSurfaceMaterial = nullptr;
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
	sDefaultSurfaceMaterial.reset();
}

const Ref<Texture>& FAssetLibrary::GetDefaultWhiteTexture2D()
{
	if (!sDefaultWhiteTexture)
	{
		FTextureDesc TextureDesc = FTextureDesc::Make2D(2, 2, PF_R8G8B8A8, false, Math::FLinearColor(1,1,1,1));
		FSamplerDesc SamplerDesc;
		sDefaultWhiteTexture = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
		sDefaultWhiteTexture->GetResource()->ClearAllData();
	}
	return sDefaultWhiteTexture;
}

const Ref<Texture>& FAssetLibrary::GetDefaultBlackTexture2D()
{
	if (!sDefaultWhiteTexture)
	{
		FTextureDesc TextureDesc = FTextureDesc::Make2D(2, 2, PF_R8G8B8A8, false, Math::FLinearColor(0, 0, 0, 1));
		FSamplerDesc SamplerDesc;
		sDefaultWhiteTexture = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
		sDefaultWhiteTexture->GetResource()->ClearAllData();
	}
	return sDefaultWhiteTexture;
}

const Ref<Texture>& FAssetLibrary::GetDefaultNormalTexture2D()
{
	if (!sDefaultNormalTexture)
	{
		FTextureDesc TextureDesc = FTextureDesc::Make2D(2, 2, PF_RGB8, false, Math::FLinearColor(0.5f, 0.5f, 1.0f));
		FSamplerDesc SamplerDesc;
		sDefaultNormalTexture = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
		sDefaultNormalTexture->GetResource()->ClearAllData();
	}
	return sDefaultNormalTexture;
}

const Ref<SurfaceMaterial>& FAssetLibrary::GetDefaultSurfaceMaterial()
{
	if (!sDefaultSurfaceMaterial)
	{
		sDefaultSurfaceMaterial = CreateRef<SurfaceMaterial>();
		sDefaultSurfaceMaterial->Compile();
	}
	return sDefaultSurfaceMaterial;
}

Ref<StaticMesh> FAssetLibrary::CreateBasicGeometry(EBasicGeometry InKind, const Ref<SurfaceMaterial>& InMaterial)
{
	const Ref<SurfaceMaterial>& GeoMat = InMaterial ? InMaterial : GetDefaultSurfaceMaterial();
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

FTextureStorage FAssetLibrary::ImportTexture(const FPath& InPath)
{
	FTextureStorage Result;
	if (FSTBImage2D image = FSTBImage::ImportImage2D(InPath); image.Valid())
	{
		Result.Name = InPath.Name();
		Result.TextureDesc = FTextureDesc::Make2D(image.Width(), image.Height(), image.Format(), false);
		Result.ImageData.Resize(1, 1);
		FBuffer& ImageBuffer = Result.ImageData.At(0, 0);
		ImageBuffer.Allocate(image.Width() * image.Height() * image.Channels() * image.PixelDepth());
		memcpy(ImageBuffer.Data(), image.Data(), ImageBuffer.Size());
	}
	return Result;
}

FTextureStorage FAssetLibrary::ImportTextureCube(const FPath& InPathPX, const FPath& InPathNX, const FPath& InPathPY, const FPath& InPathNY, const FPath& InPathPZ, const FPath& InPathNZ)
{
	FTextureStorage Result;
	std::vector<FSTBImage2D> images(6);

	images[0] = FSTBImage::ImportImage2D(InPathPX);
	images[1] = FSTBImage::ImportImage2D(InPathNX);
	images[2] = FSTBImage::ImportImage2D(InPathPY);
	images[3] = FSTBImage::ImportImage2D(InPathNY);
	images[4] = FSTBImage::ImportImage2D(InPathPZ);
	images[5] = FSTBImage::ImportImage2D(InPathNZ);

	REV_CORE_ASSERT(FSTBImage2D::SameSizeAndFormat(images[0], images[1]));
	REV_CORE_ASSERT(FSTBImage2D::SameSizeAndFormat(images[0], images[2]));
	REV_CORE_ASSERT(FSTBImage2D::SameSizeAndFormat(images[0], images[3]));
	REV_CORE_ASSERT(FSTBImage2D::SameSizeAndFormat(images[0], images[4]));
	REV_CORE_ASSERT(FSTBImage2D::SameSizeAndFormat(images[0], images[5]));

	Result.Name = "CubeTexture";
	Result.TextureDesc = FTextureDesc::MakeCube(images[0].Width(), images[0].Height(), images[0].Format(), false);
	Result.ImageData.Resize(1, 6);
	uint32 ImageDataSize = images[0].Width() * images[0].Height() * images[0].Channels() * images[0].PixelDepth() / 8;
	for (uint16 i = 0; i < 6; i++)
	{
		FBuffer& ImageBuffer = Result.ImageData.At(0, i);
		ImageBuffer.Allocate(ImageDataSize);
		memcpy(ImageBuffer.Data(), images[i].Data(), ImageDataSize);
	}


	return Result;
}

FModelImportResult FAssetLibrary::ImportModel(const FPath& InPath)
{
	if (InPath.Extension() == ".gltf" || InPath.Extension() == ".glb")
	{
		return FGLTFUtils::ImportModel(InPath);
	}

	return FModelImportResult();
}


}