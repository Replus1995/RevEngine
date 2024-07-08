#include "OpenGLShader.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Clock.h"

#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace fs = std::filesystem;

namespace Rev
{

FOpenGLShader::FOpenGLShader(ERHIShaderStage InStage, std::vector<uint32_t> InCompiledData)
	: FRHIShader(InStage)
{
	GLenum GLStage = GetOpenGLStage(InStage);
	mHandle = glCreateShader(GLStage);
	glShaderBinary(1, &mHandle, GL_SHADER_BINARY_FORMAT_SPIR_V, InCompiledData.data(), InCompiledData.size() * sizeof(uint32_t));
	glSpecializeShader(mHandle, "main", 0, nullptr, nullptr);
}

FOpenGLShader::~FOpenGLShader()
{
	glDeleteShader(mHandle);
}

GLenum FOpenGLShader::GetOpenGLStage(ERHIShaderStage InStage)
{
	switch (InStage)
	{
	case ERHIShaderStage::Vertex:
		return GL_VERTEX_SHADER;
	case ERHIShaderStage::Hull:
	case ERHIShaderStage::Domain:
		break;
	case ERHIShaderStage::Pixel:
		return GL_FRAGMENT_SHADER;
	case ERHIShaderStage::Geometry:
	case ERHIShaderStage::Compute:
	default:
		break;
	}
	RE_CORE_ASSERT(false);
	return 0;
}

namespace
{
	static shaderc_shader_kind StageToShaderC(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
		case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
		}
		RE_CORE_ASSERT(false);
		return (shaderc_shader_kind)0;
	}
}


FOpenGLShaderProgram::FOpenGLShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders)
	: FRHIShaderProgram(InName)
{
	GLuint ProgramHandle = glCreateProgram();

	std::vector<GLuint> ShaderHandles;
	for (uint8 i = uint8(ERHIShaderStage::Vertex); i < uint8(ERHIShaderStage::Compute); i++)
	{
		auto& ShaderRef = InShaders[(ERHIShaderStage)i];
		if (ShaderRef)
		{
			GLuint ShaderHandle = *(GLuint*)ShaderRef->GetNativeHandle();
			glAttachShader(ProgramHandle, ShaderHandle);
			ShaderHandles.push_back(ShaderHandle);
		}
	}

	glLinkProgram(ProgramHandle);

	GLint isLinked;
	glGetProgramiv(ProgramHandle, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength;
		glGetProgramiv(ProgramHandle, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(ProgramHandle, maxLength, &maxLength, infoLog.data());
		RE_CORE_ERROR("Shader linking failed ({0}):\n{1}", InName.c_str(), infoLog.data());

		glDeleteProgram(ProgramHandle);
	}

	for (auto ShaderHandle : ShaderHandles)
	{
		glDetachShader(ProgramHandle, ShaderHandle);
	}

	mHandle = ProgramHandle;
}

FOpenGLShaderProgram::~FOpenGLShaderProgram()
{
	glDeleteProgram(mHandle);
}

void FOpenGLShaderProgram::Bind() const
{
	glUseProgram(mHandle);
}

void FOpenGLShaderProgram::Unbind() const
{
	glUseProgram(0);
}

uint16 FOpenGLShaderProgram::GetUniformLocation(std::string_view name)
{
	return uint16(glGetUniformLocation(mHandle, name.data()));
}

void FOpenGLShaderProgram::SetUniform(uint16 location, int value)
{
	glUniform1i(location, value);
}

void FOpenGLShaderProgram::SetUniform(uint16 location, float value)
{
	glUniform1f(location, value);
}

void FOpenGLShaderProgram::SetUniform(uint16 location, const Math::FVector2& value)
{
	glUniform2f(location, value.X, value.Y);
}

void FOpenGLShaderProgram::SetUniform(uint16 location, const Math::FVector3& value)
{
	glUniform3f(location, value.X, value.Y, value.Y);
}

void FOpenGLShaderProgram::SetUniform(uint16 location, const Math::FVector4& value)
{
	glUniform4f(location, value.X, value.Y, value.Z, value.W);
}

void FOpenGLShaderProgram::SetUniform(uint16 location, const Math::FMatrix3& value)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, value.Data());
}

void FOpenGLShaderProgram::SetUniform(uint16 location, const Math::FMatrix4& value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, value.Data());
}

void FOpenGLShaderProgram::SetUniformArray(uint16 location, const int* values, uint32_t count)
{
	glUniform1iv(location, count, values);
}

}