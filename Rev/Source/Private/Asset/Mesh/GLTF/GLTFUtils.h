#pragma once
#include <tiny_gltf.h>
#include "Rev/Core/Buffer.h"
#include "Rev/Asset/Mesh/MeshPrimitiveStorage.h"

namespace Rev
{

class FGLTFUtils
{
public:
	static uint32 GetComponentSize(int InComponentType);
	static EMeshDrawMode GetDrawMode(int InDrawMode);

	static Ref<FMeshPrimitiveStorage> LoadMeshPrimitive(const tinygltf::Primitive& InPrimitive, const tinygltf::Model& InModel);

};



}