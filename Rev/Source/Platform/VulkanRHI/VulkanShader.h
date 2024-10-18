#pragma once
#include "Rev/Render/RHI/RHIShader.h"
#include "VulkanPipeline.h"
#include "../Shaderc/ShadercFactory.h"


namespace Rev
{
class FVulkanContext;
class FVulkanRenderPass;
class FVulkanPrimitive;

struct FVulkanUniformInfo : public FRHIUniformInfo
{
	VkShaderStageFlags StageFlags = 0;

	FVulkanUniformInfo(const FRHIUniformInfo& InInfo, VkShaderStageFlags InStageFlags) : FRHIUniformInfo(InInfo) , StageFlags(InStageFlags) {}
};


class FVulkanShader : public FRHIShader
{
public:
	FVulkanShader(const FShadercCompiledData& InCompiledData);
	virtual ~FVulkanShader();
	virtual const void* GetNativeHandle() const override { return mModule; }

	static VkShaderStageFlagBits TranslateShaderStage(ERHIShaderStage InStage);

	VkShaderStageFlagBits GetStageFlag() const { return mStageFlag; }
	const std::vector<FRHIUniformInfo>& GetStageUniforms() const { return mStageUniforms; }

private:
	VkShaderModule mModule = VK_NULL_HANDLE;
	std::string mDebugName;
	VkShaderStageFlagBits mStageFlag = VK_SHADER_STAGE_ALL;
	std::vector<FRHIUniformInfo> mStageUniforms;
};

class FVulkanShaderProgram : public FRHIShaderProgram
{
public:
	FVulkanShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders);
	virtual ~FVulkanShaderProgram();
	virtual const void* GetNativeHandle() const override { return nullptr; }

	virtual uint16 GetUniformLocation(std::string_view name) override { return 0; };

	virtual void SetUniform(uint16 location, int value) override {};
	virtual void SetUniform(uint16 location, float value) override {};
	virtual void SetUniform(uint16 location, const Math::FVector2& value) override {};
	virtual void SetUniform(uint16 location, const Math::FVector3& value) override {};
	virtual void SetUniform(uint16 location, const Math::FVector4& value) override {};
	virtual void SetUniform(uint16 location, const Math::FMatrix3& value) override {};
	virtual void SetUniform(uint16 location, const Math::FMatrix4& value) override {};

	virtual void SetUniformArray(uint16 location, const int* values, uint32_t count) override {};

	void PrepareDraw(FVulkanContext* Context, const FVulkanRenderPass* RenderPass, const FVulkanPrimitive* Primitive);

	const std::vector<FVulkanUniformInfo>& GetProgramUniforms() const { return mProgramUniforms; };
	std::vector<VkPipelineShaderStageCreateInfo> GenShaderStageInfo();
	std::vector<VkDescriptorSetLayoutBinding> GenLayoutBindings();

private:
	void UpdateProgramUniforms();

private:
	FRHIGraphicsShaders mShaders;
	std::vector<FVulkanUniformInfo> mProgramUniforms;
};

}