#pragma once
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RHI/RHIShaderCompile.h"
#include "Rev/Core/Buffer.h"
#include <map>
#include "Core/VulkanDefines.h"
#include "VulkanPipeline.h"
#include "../Shaderc/ShadercFactory.h"


namespace Rev
{
class FVulkanRenderTarget;
class FVulkanPrimitive;
class FVulkanShader : public FRHIShader
{
public:
	FVulkanShader(const FShadercCompiledData& InCompiledData);
	virtual ~FVulkanShader();
	virtual const void* GetNativeHandle() const override { return mModule; }

	static VkShaderStageFlagBits TranslateShaderStage(ERHIShaderStage InStage);

	VkShaderStageFlagBits GetStageFlags() const { return mStageFlags; }
	const std::vector<FRHIUniformInfo>& GetStageUniforms() const { return mStageUniforms; }

private:
	VkShaderModule mModule = VK_NULL_HANDLE;
	std::string mDebugName;
	VkShaderStageFlagBits mStageFlags = VK_SHADER_STAGE_ALL;
	std::vector<FRHIUniformInfo> mStageUniforms;
};

class FVulkanShaderProgram : public FRHIShaderProgram
{
public:
	FVulkanShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders);
	virtual ~FVulkanShaderProgram();
	virtual const void* GetNativeHandle() const override { return mPipeline.GetPipeline(); }

	virtual uint16 GetUniformLocation(std::string_view name) override { return 0; };

	virtual void SetUniform(uint16 location, int value) override {};
	virtual void SetUniform(uint16 location, float value) override {};
	virtual void SetUniform(uint16 location, const Math::FVector2& value) override {};
	virtual void SetUniform(uint16 location, const Math::FVector3& value) override {};
	virtual void SetUniform(uint16 location, const Math::FVector4& value) override {};
	virtual void SetUniform(uint16 location, const Math::FMatrix3& value) override {};
	virtual void SetUniform(uint16 location, const Math::FMatrix4& value) override {};

	virtual void SetUniformArray(uint16 location, const int* values, uint32_t count) override {};

	void PrepareDraw(const FVulkanRenderTarget* RenderTarget, const FVulkanPrimitive* Primitive);

private:
	static std::vector<VkPipelineShaderStageCreateInfo> MakeShaderStageInfo(const FRHIGraphicsShaders& InShaders);
	std::vector<VkDescriptorSetLayoutBinding> MakeLayoutBindings(const FRHIGraphicsShaders& InShaders);

	VkDescriptorSet GetDescriptorSet();

private:
	FVulkanPipeline mPipeline;
	FRHIGraphicsShaders mShaders;
	uint64 mIuputDescHashCache = 0;
	uint32 mNumColorTargetsCache = 0;
	std::vector<FRHIUniformInfo> mProgramUniforms;
};

}