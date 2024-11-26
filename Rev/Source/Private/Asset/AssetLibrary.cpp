#include "Rev/Asset/AssetLibrary.h"
#include "Rev/HAL/FIleManager.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHIState.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Material/PBRMaterial.h"
#include "Rev/Render/Resource/RenderResource.h"

#include "./BasicGeometry/PlaneGeometry.hpp"
#include "./BasicGeometry/BoxGeometry.hpp"
#include "./GLTF/GLTFUtils.h"
#include "./STB/STBImage.h"

#include <filesystem>

namespace Rev
{

static Ref<FMaterial> sDefaultSurfaceMaterial = nullptr;

void FAssetLibrary::Init()
{
	IFileManager::Get().Mount("/Engine", (std::filesystem::current_path() / "Engine").generic_string().c_str());
}

void FAssetLibrary::Shutdown()
{
	sDefaultSurfaceMaterial.reset();
}

const Ref<FMaterial>& FAssetLibrary::GetDefaultSurfaceMaterial()
{
	if (!sDefaultSurfaceMaterial)
	{
		sDefaultSurfaceMaterial = CreateRef<FPBRMaterial>();
		sDefaultSurfaceMaterial->Compile();
	}
	return sDefaultSurfaceMaterial;
}

Ref<FStaticMesh> FAssetLibrary::CreateBasicGeometry(EBasicGeometry InKind, const Ref<FMaterial>& InMaterial)
{
	const Ref<FMaterial>& GeoMat = InMaterial ? InMaterial : GetDefaultSurfaceMaterial();
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

FTextureStorage FAssetLibrary::ImportTexture(const char* InPath)
{
	FTextureStorage Result;
	if (FSTBImage2D image = FSTBImage::ImportImage2D(InPath); image.Valid())
	{
		Result.TextureDesc = FRHITextureDesc::Create2D(image.Width(), image.Height(), image.Format());
		Result.ImageData.Resize(1, 1);

		FBuffer& ImageBuffer = Result.ImageData.At(0, 0);
		ImageBuffer.Allocate(image.Width() * image.Height() * image.Channels() * image.PixelDepth());
		memcpy(ImageBuffer.Data(), image.Data(), ImageBuffer.Size());
	}
	return Result;
}

FTextureStorage FAssetLibrary::ImportTextureCube(const char* InPathPX, const char* InPathNX, const char* InPathPY, const char* InPathNY, const char* InPathPZ, const char* InPathNZ, bool bSRG)
{
	FTextureStorage Result;
	std::vector<FSTBImage2D> images(6);

	images[0] = FSTBImage::ImportImage2D(InPathPX);
	REV_CORE_ASSERT(images[0].Width() == images[0].Height());

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

	Result.TextureDesc = FRHITextureDesc::CreateCube(images[0].Width(), images[0].Format());
	Result.TextureDesc.AddFlags(ETextureCreateFlags::ShaderResource);
	if(bSRG)
		Result.TextureDesc.AddFlags(ETextureCreateFlags::SRGB);

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

FModelImportResult FAssetLibrary::ImportModel(const char* InPath)
{
	std::filesystem::path ModelPath(InPath);
	std::string ModelExt = ModelPath.extension().generic_string();

	if (ModelExt == ".gltf" || ModelExt == ".glb")
	{
		return FGLTFUtils::ImportModel(InPath, ModelExt == ".glb");
	}

	return FModelImportResult();
}


}