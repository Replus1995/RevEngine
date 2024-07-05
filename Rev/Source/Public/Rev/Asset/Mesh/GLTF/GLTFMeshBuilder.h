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
	GLTFMeshBuilder(const Path& InPath);
	~GLTFMeshBuilder();

	//const std::vector<Ref<Mesh>>& BuildMeshData(bool DumpInfo = false);

private:
	Path mFilePath;
	std::vector<Ref<Mesh>> mMeshVec;
};

}