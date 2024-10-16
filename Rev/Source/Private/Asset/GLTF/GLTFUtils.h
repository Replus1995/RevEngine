#pragma once
#include <tiny_gltf.h>
#include "Rev/Core/Buffer.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Asset/MeshPrimitiveStorage.h"
#include "Rev/Asset/MeshStorage.h"
#include "Rev/Asset/TextureStorage.h"
#include "Rev/Asset/MaterialStorage.h"

namespace Rev
{

class FGLTFUtils
{
public:
	static uint32 GetIndexStride(int InComponentType);

	static EPrimitiveTopology TranslatePrimitiveTopology(int InPrimitiveTopology);
	static EPixelFormat TranslateImageFormat(const tinygltf::Image& InImage);
	static FSamplerDesc TranslateSampler(const tinygltf::Sampler& InSampler);
	static Math::FLinearColor TranslateColor(const std::vector<double>& InColor);
	static Math::FVector3 TranslateVector3(const std::vector<double>& InVector3);

	static Ref<FMeshPrimitiveStorage> ImportMeshPrimitive(const tinygltf::Primitive& InPrimitive, const tinygltf::Model& InModel);
	static Ref<FStaticMeshStorage> ImportStaticMesh(const tinygltf::Mesh& InMesh, const tinygltf::Model& InModel, const std::vector<Ref<FSurfaceMaterialStorage>>& InMaterials);
	static Ref<FTextureStorage> ImportTexture(const tinygltf::Texture& InTexture, const tinygltf::Model& InModel);
	static Ref<FSurfaceMaterialStorage> ImportMaterial(const tinygltf::Material& InMaterial, const tinygltf::Model& InModel, const std::vector<Ref<FTextureStorage>>& InTextures);
	static FModelImportResult ImportModel(const FPath& InPath, bool DumpInfo = false);
};



}