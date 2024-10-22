#pragma once
#include "Rev/Render/RHI/RHIShader.h"
#include "VulkanPipeline.h"
#include "../Shaderc/ShadercFactory.h"


namespace Rev
{
class FVulkanContext;
class FVulkanRenderPass;
class FVulkanPrimitive;

struct FVulkanUniformInfo : public FRHIShaderUniform
{
	VkShaderStageFlags StageFlags = 0;

	FVulkanUniformInfo(const FRHIShaderUniform& InInfo, VkShaderStageFlags InStageFlags) : FRHIShaderUniform(InInfo) , StageFlags(InStageFlags) {}
};


class FVulkanShader : public FRHIShader
{
public:
	FVulkanShader(const FShadercCompiledData& InCompiledData);
	virtual ~FVulkanShader();
	virtual const void* GetNativeHandle() const override { return mModule; }

	static VkShaderStageFlagBits TranslateShaderStage(ERHIShaderStage InStage);

	uint32 GetHash() const { return mHash; }
	VkShaderStageFlagBits GetStageFlag() const { return mStageFlag; }
	const std::vector<FRHIShaderUniform>& GetStageUniforms() const { return mStageUniforms; }

private:
	std::string mName;
	uint32 mHash;
	VkShaderStageFlagBits mStageFlag = VK_SHADER_STAGE_ALL;
	VkShaderModule mModule = VK_NULL_HANDLE;
	std::vector<FRHIShaderUniform> mStageUniforms;
};

class FVulkanShaderProgram : public FRHIShaderProgram
{
public:
	FVulkanShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders);
	virtual ~FVulkanShaderProgram();
	virtual const void* GetNativeHandle() const override { return nullptr; }

	const FRHIGraphicsShaders& GetShaders() const { return mShaders; }
	const std::vector<FVulkanUniformInfo>& GetProgramUniforms() const { return mProgramUniforms; };
	std::vector<VkPipelineShaderStageCreateInfo> GenShaderStageInfo() const;
	std::vector<VkDescriptorSetLayoutBinding> GenLayoutBindings() const;

private:
	void UpdateProgramUniforms();

private:
	FRHIGraphicsShaders mShaders;
	std::vector<FVulkanUniformInfo> mProgramUniforms;
};

}