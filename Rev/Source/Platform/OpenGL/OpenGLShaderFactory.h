#pragma once
#include "OpenGLShader.h"
#include "Rev/Core/FileSystem.h"
#include "Platform/Shaderc/ShadercFactory.h"

namespace Rev
{

class FOpenGLShaderFactory
{
public:
	static FRHIGraphicsShaders CreateGraphicsShaders(const FShadercCompiledData& InData);
	//CreateComputeShader
	static Ref<FOpenGLShaderProgram> CreateShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders);
};

}