#pragma once
#include <tiny_gltf.h>
#include "Rev/Core/Buffer.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Asset/TextureStorage.h"
#include "Rev/Asset/MaterialStorage.h"
#include "Rev/Asset/AssetLibrary.h"

namespace Rev
{

class FStaticMesh;
class FGLTFUtils
{
public:
	static uint32 GetIndexStride(int InComponentType);

	static EPrimitiveTopology TranslatePrimitiveTopology(int InPrimitiveTopology);
	static EPixelFormat TranslateImageFormat(const tinygltf::Image& InImage);
	static FRHISamplerStateDesc TranslateSampler(const tinygltf::Sampler& InSampler);
	static Math::FLinearColor TranslateColor(const std::vector<double>& InColor);
	static Math::FVector3 TranslateVector3(const std::vector<double>& InVector3);

	static Ref<FStaticMesh> ImportMeshPrimitive(const tinygltf::Primitive& InPrimitive, const tinygltf::Model& InModel, const std::vector<Ref<FMaterial>>& InMaterials);
	static std::vector<Ref<FStaticMesh>> ImportStaticMesh(const tinygltf::Mesh& InMesh, const tinygltf::Model& InModel, const std::vector<Ref<FMaterial>>& InMaterials);
	static Ref<FTextureStorage> ImportTexture(const tinygltf::Texture& InTexture, const tinygltf::Model& InModel);
	static Ref<FMaterialStorage> ImportMaterial(const tinygltf::Material& InMaterial, const tinygltf::Model& InModel, const std::vector<Ref<FTextureStorage>>& InTextures);
	static FModelImportResult ImportModel(const FPath& InPath, bool DumpInfo = false);
};



}