#pragma once
#include <tiny_gltf.h>
#include "Rev/Core/Buffer.h"
#include "Rev/Asset/MeshPrimitiveStorage.h"
#include "Rev/Asset/MeshStorage.h"
#include "Rev/Asset/TextureStorage.h"

namespace Rev
{

class FGLTFUtils
{
public:
	static uint32 GetComponentSize(int InComponentType);
	static EMeshDrawMode TranslateDrawMode(int InDrawMode);
	static EPixelFormat TranslateImageFormat(const tinygltf::Image& InImage);

	static Ref<FMeshPrimitiveStorage> ImportMeshPrimitive(const tinygltf::Primitive& InPrimitive, const tinygltf::Model& InModel);
	static Ref<FStaticMeshStorage> ImportStaticMesh(const tinygltf::Mesh& InMesh, const tinygltf::Model& InModel);
	static Ref<FTextureStorage> LoadTexture(const tinygltf::Texture& InTexture, const tinygltf::Model& InModel);

};



}