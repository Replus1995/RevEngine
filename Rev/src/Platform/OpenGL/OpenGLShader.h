#pragma once
#include "Pine/Render/Resource/Shader.h"
#include <map>
#include <glad/glad.h>

namespace Pine
{

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& name, const std::string& vertSrc, const std::string& fragSrc);
	virtual ~OpenGLShader();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual ShaderUniformLocation GetUniformLocation(std::string_view name) override;

	virtual void SetUniform(ShaderUniformLocation location, int value) override;
	virtual void SetUniform(ShaderUniformLocation location, float value) override;
	virtual void SetUniform(ShaderUniformLocation location, const FVector2& value) override;
	virtual void SetUniform(ShaderUniformLocation location, const FVector3& value) override;
	virtual void SetUniform(ShaderUniformLocation location, const FVector4& value) override;
	virtual void SetUniform(ShaderUniformLocation location, const FMatrix3& value) override;
	virtual void SetUniform(ShaderUniformLocation location, const FMatrix4& value) override;

	virtual void SetUniformArray(ShaderUniformLocation location, int* values, uint32_t count) override;

	virtual const std::string& GetName() const override { return mName; }
private:
	std::string ReadFile(const std::string& filepath);
	std::map<GLenum, std::string> PreProcess(const std::string& source);
	void CompileOrGetOpenGLBinaries(const std::map<GLenum, std::string>& shaderSources);
	void CreateProgram();
	void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);

private:
	static GLenum StageFromStr(const std::string& type);
	static const char* StageToStr(GLenum stage);

	static const char* GetCacheDirectory();
	static void CreateCacheDirectoryIfNeeded();
	static const char* GetCachedFileExtension(uint32_t stage);

private:
	uint32_t mHandle = 0;
	std::string mFilePath;
	std::string mName;
	std::unordered_map<GLenum, std::vector<uint32_t>> mOpenGLSPIRV;
};

}