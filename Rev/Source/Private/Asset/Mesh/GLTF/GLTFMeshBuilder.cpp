#include "Rev/Asset/Mesh/GLTF/GLTFMeshBuilder.h"
#include "Rev/Core/Log.h"
#include "Rev/Render/Mesh/MeshUtils.h"
#include "Rev/Render/RHI/RHIVertexBuffer.h"
#include <iostream>

#include "GLTFDebug.h"
#include "GLTFUtils.h"

#define TINYGLTF_USE_CPP14
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <tiny_gltf.h>

namespace Rev
{

GLTFMeshBuilder::GLTFMeshBuilder(const Path& InPath)
	: mFilePath(InPath)
{
}

GLTFMeshBuilder::~GLTFMeshBuilder()
{
}

//const std::vector<Ref<Mesh>>& GLTFMeshBuilder::BuildMeshData(bool DumpInfo)
//{
//    if(!mMeshVec.empty())
//        return mMeshVec;
//
//	tinygltf::Model model;
//	tinygltf::TinyGLTF ctx;
//    std::string err;
//    std::string warn;
//
//    std::string NativePath = mFilePath.ToNative();
//    bool ret = false;
//    if (mFilePath.Extension().compare("glb") == 0) {
//
//        RE_CORE_INFO("Reading binary glTF {0}", NativePath);
//        // assume binary glTF.
//        ret = ctx.LoadBinaryFromFile(&model, &err, &warn, NativePath.c_str());
//    }
//    else {
//        RE_CORE_INFO("Reading ASCII glTF {0}", NativePath);
//        // assume ascii glTF.
//        ret = ctx.LoadASCIIFromFile(&model, &err, &warn, NativePath.c_str());
//    }
//
//    if (!warn.empty()) {
//        RE_CORE_WARN("glTF load warning: {}", warn);
//    }
//
//    if (!err.empty()) {
//        RE_CORE_ERROR("glTF load error: {}", err);
//    }
//
//    if (!ret) {
//        RE_CORE_ERROR("glTF load failed: {}", NativePath);
//        return mMeshVec;
//    }
//
//    if (DumpInfo)
//    {
//        FGLTFDebug::DumpModelInfo(model);
//    }
//
//}


}