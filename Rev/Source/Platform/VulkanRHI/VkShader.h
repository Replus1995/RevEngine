#pragma once
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Core/Buffer.h"
#include <map>
#include <vulkan/vulkan.h>

namespace Rev
{

class FVulkanShader : public FRHIShader
{
public:
	FVulkanShader(ERHIShaderStage InStage, const FBuffer& InCompiledData);
	virtual ~FVulkanShader();
	virtual const void* GetNativeHandle() const override { return mModule; }

private:
	static VkShaderStageFlagBits TranslateShaderStage(ERHIShaderStage InStage);
private:
	VkShaderModule mModule = VK_NULL_HANDLE;
	//VkPipelineShaderStageCreateInfo mStageCreateInfo;
};

class FVulkanShaderProgram : public FRHIShaderProgram
{
public:
	FVulkanShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders);
	virtual ~FVulkanShaderProgram();
	virtual const void* GetNativeHandle() const override { return &mHandle; }

	virtual uint16 GetUniformLocation(std::string_view name) override;

	virtual void SetUniform(uint16 location, int value) override;
	virtual void SetUniform(uint16 location, float value) override;
	virtual void SetUniform(uint16 location, const Math::FVector2& value) override;
	virtual void SetUniform(uint16 location, const Math::FVector3& value) override;
	virtual void SetUniform(uint16 location, const Math::FVector4& value) override;
	virtual void SetUniform(uint16 location, const Math::FMatrix3& value) override;
	virtual void SetUniform(uint16 location, const Math::FMatrix4& value) override;

	virtual void SetUniformArray(uint16 location, const int* values, uint32_t count) override;

private:
	uint32 mHandle;
};

}