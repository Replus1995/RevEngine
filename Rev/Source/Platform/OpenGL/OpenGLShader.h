#pragma once
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Core/Buffer.h"
#include <map>
#include <glad/gl.h>

namespace Rev
{

class FOpenGLShader : public FRHIShader
{
public:
	FOpenGLShader(ERHIShaderStage InStage, const FBuffer& InCompiledData);
	virtual ~FOpenGLShader();
	virtual void* GetNativeHandle() override { return &mHandle; }

private:
	static GLenum TranslateShaderStage(ERHIShaderStage InStage);
private:
	GLuint mHandle = 0;
};

class FOpenGLShaderProgram : public FRHIShaderProgram
{
public:
	FOpenGLShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders);
	virtual ~FOpenGLShaderProgram();
	virtual void* GetNativeHandle() override { return &mHandle; }

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual uint16 GetUniformLocation(std::string_view name) override;

	virtual void SetUniform(uint16 location, int value) override;
	virtual void SetUniform(uint16 location, float value) override;
	virtual void SetUniform(uint16 location, const Math::FVector2& value) override;
	virtual void SetUniform(uint16 location, const Math::FVector3& value) override;
	virtual void SetUniform(uint16 location, const Math::FVector4& value) override;
	virtual void SetUniform(uint16 location, const Math::FMatrix3& value) override;
	virtual void SetUniform(uint16 location, const Math::FMatrix4& value) override;
	virtual void SetUniform(uint16 location, const Ref<FRHITexture>& value) override;

	virtual void SetUniformArray(uint16 location, const int* values, uint32_t count) override;

private:
	uint32_t mHandle = 0;
};

}