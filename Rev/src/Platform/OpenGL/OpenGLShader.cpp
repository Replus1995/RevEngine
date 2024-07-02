#include "pinepch.h"
#include "OpenGLShader.h"
#include "Pine/Core/Assert.h"
#include "Pine/Core/Clock.h"

#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace fs = std::filesystem;

namespace Pine
{

namespace
{
	static shaderc_shader_kind StageToShaderC(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
		case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
		}
		PE_CORE_ASSERT(false);
		return (shaderc_shader_kind)0;
	}
}

OpenGLShader::OpenGLShader(const std::string& filepath)
	: mFilePath(filepath)
{
	CreateCacheDirectoryIfNeeded();

	std::string content = ReadFile(filepath);
	auto sources = PreProcess(content);
	{
		Clock timer;
		CompileOrGetOpenGLBinaries(sources);
		CreateProgram();
		PE_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
	}

	mName = fs::path(filepath).stem().generic_u8string();
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertSrc, const std::string& fragSrc)
	: mName(name)
{
	CreateCacheDirectoryIfNeeded();

	std::map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER] = vertSrc;
	sources[GL_FRAGMENT_SHADER] = fragSrc;

	{
		Clock timer;
		CompileOrGetOpenGLBinaries(sources);
		CreateProgram();
		PE_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
	}
}

OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(mHandle);
}

std::string OpenGLShader::ReadFile(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
	if (in)
	{
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		if (size != -1)
		{
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], size);
		}
		else
		{
			PE_CORE_ERROR("Could not read from file '{0}'", filepath);
		}
	}
	else
	{
		PE_CORE_ERROR("Could not open file '{0}'", filepath);
	}

	return result;
}

std::map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
{
	std::map<GLenum, std::string> shaderSources;

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
	while (pos != std::string::npos)
	{
		size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
		PE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
		std::string type = source.substr(begin, eol - begin);
		PE_CORE_ASSERT(StageFromStr(type), "Invalid shader type specified");

		size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
		PE_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
		pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

		shaderSources[StageFromStr(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
	}

	return shaderSources;
}

void OpenGLShader::CompileOrGetOpenGLBinaries(const std::map<GLenum, std::string>& shaderSources)
{
	auto& shaderData = mOpenGLSPIRV;

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
	const bool optimize = false;
	if (optimize)
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

	fs::path cacheDirectory = GetCacheDirectory();

	shaderData.clear();
	for (auto&& [stage, source] : shaderSources)
	{
		fs::path shaderFilePath(mFilePath);
		fs::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GetCachedFileExtension(stage));

		std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
		if (in.is_open())
		{
			in.seekg(0, std::ios::end);
			auto size = in.tellg();
			in.seekg(0, std::ios::beg);

			auto& data = shaderData[stage];
			data.resize(size / sizeof(uint32_t));
			in.read((char*)data.data(), size);
		}
		else
		{
			shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, StageToShaderC(stage), mFilePath.c_str(), options);
			if (module.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				PE_CORE_ERROR(module.GetErrorMessage());
				PE_CORE_ASSERT(false, "");
			}

			shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

			std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
			if (out.is_open())
			{
				auto& data = shaderData[stage];
				out.write((char*)data.data(), data.size() * sizeof(uint32_t));
				out.flush();
				out.close();
			}
		}
	}

	for (auto&& [stage, data] : shaderData)
		Reflect(stage, data);
}

void OpenGLShader::CreateProgram()
{
	GLuint program = glCreateProgram();

	std::vector<GLuint> shaderIDs;
	for (auto&& [stage, spirv] : mOpenGLSPIRV)
	{
		GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
		glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
		glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
		glAttachShader(program, shaderID);
	}

	glLinkProgram(program);

	GLint isLinked;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
		PE_CORE_ERROR("Shader linking failed ({0}):\n{1}", mFilePath, infoLog.data());

		glDeleteProgram(program);

		for (auto id : shaderIDs)
			glDeleteShader(id);
	}

	for (auto id : shaderIDs)
	{
		glDetachShader(program, id);
		glDeleteShader(id);
	}

	mHandle = program;
}

void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
{
	spirv_cross::Compiler compiler(shaderData);
	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	PE_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", StageToStr(stage), mFilePath);
	PE_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
	PE_CORE_TRACE("    {0} resources", resources.sampled_images.size());

	PE_CORE_TRACE("Uniform buffers:");
	for (const auto& resource : resources.uniform_buffers)
	{
		const auto& bufferType = compiler.get_type(resource.base_type_id);
		size_t bufferSize = compiler.get_declared_struct_size(bufferType);
		uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
		int memberCount = bufferType.member_types.size();

		PE_CORE_TRACE("  {0}", resource.name);
		PE_CORE_TRACE("    Size = {0}", bufferSize);
		PE_CORE_TRACE("    Binding = {0}", binding);
		PE_CORE_TRACE("    Members = {0}", memberCount);
	}
}

void OpenGLShader::Bind() const
{
	glUseProgram(mHandle);
}

void OpenGLShader::Unbind() const
{
	glUseProgram(0);
}

ShaderUniformLocation OpenGLShader::GetUniformLocation(std::string_view name)
{
	return ShaderUniformLocation(glGetUniformLocation(mHandle, name.data()));
}

void OpenGLShader::SetUniform(ShaderUniformLocation location, int value)
{
	glUniform1i(location, value);
}

void OpenGLShader::SetUniform(ShaderUniformLocation location, float value)
{
	glUniform1f(location, value);
}

void OpenGLShader::SetUniform(ShaderUniformLocation location, const FVector2& value)
{
	glUniform2f(location, value.X, value.Y);
}

void OpenGLShader::SetUniform(ShaderUniformLocation location, const FVector3& value)
{
	glUniform3f(location, value.X, value.Y, value.Y);
}

void OpenGLShader::SetUniform(ShaderUniformLocation location, const FVector4& value)
{
	glUniform4f(location, value.X, value.Y, value.Z, value.W);
}

void OpenGLShader::SetUniform(ShaderUniformLocation location, const FMatrix3& value)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, value.DataPtr());
}

void OpenGLShader::SetUniform(ShaderUniformLocation location, const FMatrix4& value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, value.DataPtr());
}

void OpenGLShader::SetUniformArray(ShaderUniformLocation location, int* values, uint32_t count)
{
	glUniform1iv(location, count, values);
}

GLenum OpenGLShader::StageFromStr(const std::string& type)
{
	if (type == "vertex")
		return GL_VERTEX_SHADER;
	if (type == "fragment" || type == "pixel")
		return GL_FRAGMENT_SHADER;

	PE_CORE_ASSERT(false, "Unknown shader type!");
	return 0;
}

const char* OpenGLShader::StageToStr(GLenum stage)
{
	switch (stage)
	{
	case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
	case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
	}
	PE_CORE_ASSERT(false);
	return nullptr;
}

const char* OpenGLShader::GetCacheDirectory()
{
	return "assets/cache/shader/opengl";
}

void OpenGLShader::CreateCacheDirectoryIfNeeded()
{
	std::string cacheDirectory = GetCacheDirectory();
	if (!std::filesystem::exists(cacheDirectory))
		std::filesystem::create_directories(cacheDirectory);
}

const char* OpenGLShader::GetCachedFileExtension(uint32_t stage)
{
	switch (stage)
	{
	case GL_VERTEX_SHADER:    return ".cached.vert";
	case GL_FRAGMENT_SHADER:  return ".cached.frag";
	}
	PE_CORE_ASSERT(false);
	return "";
}



}