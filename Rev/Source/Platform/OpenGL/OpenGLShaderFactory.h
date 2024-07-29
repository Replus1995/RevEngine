#pragma once
#include "OpenGLShader.h"
#include "Rev/Core/FileSystem.h"
#include "Shaderc/ShadercFactory.h"

namespace Rev
{

class FOpenGLShaderFactory
{
public:
	static Ref<FOpenGLShader> CreateShader(const FShadercCompiledData& InData);
	static Ref<FOpenGLShaderProgram> CreateShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders);
};

}