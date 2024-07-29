#pragma once
#include <tiny_gltf.h>

namespace Rev
{

class FGLTFDebug
{
public:
	static void DumpModelInfo(const tinygltf::Model& model);
};

}