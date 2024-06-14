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

	virtual void SetUniform(const std::string& name, int value) override;
	virtual void SetUniform(const std::string& name, float value) override;
	virtual void SetUniform(const std::string& name, const glm::vec2& value) override;
	virtual void SetUniform(const std::string& name, const glm::vec3& value) override;
	virtual void SetUniform(const std::string& name, const glm::vec4& value) override;
	virtual void SetUniform(const std::string& name, const glm::mat4& value) override;

	virtual void SetUniformArray(const std::string& name, int* values, uint32_t count) override;

	virtual const std::string& GetName() const override { return mName; }

	void UploadUniformInt(const std::string& name, int value);
	void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
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