#include "OpenGLShaderFactory.h"

namespace Rev
{

Ref<FOpenGLShader> FOpenGLShaderFactory::CreateShader(const FShadercCompiledData& InData)
{
	return CreateRef<FOpenGLShader>(InData.Stage, InData.CompiledData);
}

Ref<FOpenGLShaderProgram> FOpenGLShaderFactory::CreateShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders)
{
	return CreateRef<FOpenGLShaderProgram>(InName, InShaders);
}

}