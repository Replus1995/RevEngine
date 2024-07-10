#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include <vector>

namespace Rev
{

class GLTFMeshBuilder
{
public:
	GLTFMeshBuilder(const FPath& InPath);
	~GLTFMeshBuilder();

	//const std::vector<Ref<Mesh>>& BuildMeshData(bool DumpInfo = false);

private:
	FPath mFilePath;
	std::vector<Ref<Mesh>> mMeshVec;
};

}