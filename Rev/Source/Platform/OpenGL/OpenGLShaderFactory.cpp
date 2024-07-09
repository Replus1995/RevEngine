#include "OpenGLShaderFactory.h"

namespace Rev
{

FRHIGraphicsShaders FOpenGLShaderFactory::CreateGraphicsShaders(const FShadercCompiledData& InData)
{
	FRHIGraphicsShaders Result;
	for (auto& [Stage, CompiledData] : InData.CompiledDataMap)
	{
		switch (Stage)
		{
		case ERHIShaderStage::Vertex:
			Result.VertexShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		case ERHIShaderStage::Hull:
			Result.HullShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		case ERHIShaderStage::Domain:
			Result.DomainShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		case ERHIShaderStage::Pixel:
			Result.PixelShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		case ERHIShaderStage::Geometry:
			Result.DomainShader = CreateRef<FOpenGLShader>(Stage, CompiledData);
			break;
		default:
			break;
		}
	}
	return Result;
}

Ref<FOpenGLShaderProgram> FOpenGLShaderFactory::CreateShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders)
{
	return CreateRef<FOpenGLShaderProgram>(InName, InShaders);
}

}