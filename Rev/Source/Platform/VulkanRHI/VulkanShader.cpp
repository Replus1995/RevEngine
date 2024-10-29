#include "VulkanShader.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanDynamicRHI.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Hash.h"

namespace Rev
{

FVulkanShader::FVulkanShader(const FShadercCompiledData& InCompiledData)
	: FRHIShader(InCompiledData.Stage)
	, mName(InCompiledData.Name)
	, mStageFlag(TranslateShaderStage(InCompiledData.Stage))
{
	REV_CORE_ASSERT(!mName.empty());
	mHash = FCityHash::Gen(mName);

	VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
	ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderModuleCreateInfo.codeSize = InCompiledData.Binary.Size();
	ShaderModuleCreateInfo.pCode = InCompiledData.Binary.DataAs<uint32_t>();

	REV_VK_CHECK_THROW(vkCreateShaderModule(FVulkanDynamicRHI::GetDevice(), &ShaderModuleCreateInfo, nullptr, &mModule), "[FVkShader] Failed to create shader module!");

	mStageUniforms = InCompiledData.Uniforms;
}

FVulkanShader::~FVulkanShader()
{
	vkDestroyShaderModule(FVulkanDynamicRHI::GetDevice(), mModule, nullptr);
}

VkShaderStageFlagBits FVulkanShader::TranslateShaderStage(EShaderStage InStage)
{
	switch (InStage)
	{
	case EShaderStage::Vertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case EShaderStage::Pixel:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case EShaderStage::Hull:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case EShaderStage::Domain:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case EShaderStage::Geometry:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	case EShaderStage::Compute:
		return VK_SHADER_STAGE_COMPUTE_BIT;
	default:
		break;
	}
	REV_CORE_ASSERT(false);
	return VkShaderStageFlagBits(0);
}

FVulkanShaderProgram::FVulkanShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders)
	: FRHIShaderProgram(InName)
    , mShaders(InShaders)
{
	UpdateProgramUniforms();
}

FVulkanShaderProgram::~FVulkanShaderProgram()
{
}

uint32 FVulkanShaderProgram::GenShaderStageInfo(VkPipelineShaderStageCreateInfo* OutInfos) const
{
	uint32 StageCount = 0;
    for (uint8 i = (uint8)EShaderStage::Vertex; i < (uint8)EShaderStage::NumGfx; i++)
    {
        const auto& pShader = mShaders[(EShaderStage)i];
        if(!pShader)
            continue;
        VkPipelineShaderStageCreateInfo& StageInfo = OutInfos[StageCount++];
		StageInfo = {};
        StageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        StageInfo.pNext = nullptr;
        StageInfo.stage = FVulkanShader::TranslateShaderStage((EShaderStage)i);
        StageInfo.module = (VkShaderModule)pShader->GetNativeHandle();
        StageInfo.pName = "main";
    }
    return StageCount;
}

uint32 FVulkanShaderProgram::GenLayoutBindings(VkDescriptorSetLayoutBinding* OutBindings) const
{
	uint32 BindingsCount = 0;
	for (const FVulkanUniformInfo& Uniform : mProgramUniforms)
	{
		switch (Uniform.Type)
		{
		case EShaderUniformType::Buffer:
		{
			VkDescriptorSetLayoutBinding& Binding = OutBindings[BindingsCount++];
			Binding = {};
			Binding.stageFlags = Uniform.StageFlags;
			Binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			Binding.binding = Uniform.Binding;
			Binding.pImmutableSamplers = NULL;
			Binding.descriptorCount = 1;
		}
		break;
		case EShaderUniformType::Texture:
		{
			VkDescriptorSetLayoutBinding& Binding = OutBindings[BindingsCount++];
			Binding.stageFlags = Uniform.StageFlags;
			Binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			Binding.binding = Uniform.Binding;
			Binding.pImmutableSamplers = NULL;
			Binding.descriptorCount = 1;

			if (Uniform.SamplerBinding >= 0)
			{
				VkDescriptorSetLayoutBinding& Binding = OutBindings[BindingsCount++];
				Binding.stageFlags = Uniform.StageFlags;
				Binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				Binding.binding = Uniform.SamplerBinding;
				Binding.pImmutableSamplers = NULL;
				Binding.descriptorCount = 1;
			}
		}
		break;
		default:
			break;
		}
	}

	return BindingsCount;
}

void FVulkanShaderProgram::UpdateProgramUniforms()
{
	mProgramUniforms.clear();

	for (uint8 i = (uint8)EShaderStage::Vertex; i < (uint8)EShaderStage::NumGfx; i++)
	{
		const auto& pShader = mShaders[(EShaderStage)i];
		if (!pShader)
			continue;
		FVulkanShader* pVulkanShader = static_cast<FVulkanShader*>(pShader.get());
		const std::vector<FRHIShaderUniform>& StageUniforms = pVulkanShader->GetStageUniforms();
		for (const FRHIShaderUniform& StageUniform : StageUniforms)
		{

			if (auto iter = std::find_if(mProgramUniforms.begin(), mProgramUniforms.end(), [&StageUniform](const FVulkanUniformInfo& Elem) { return Elem.Type == StageUniform.Type && Elem.Binding == StageUniform.Binding; });
				iter != mProgramUniforms.end())
			{
				//REV_CORE_ASSERT(iter->Type == StageUniform.Type);
				iter->StageFlags |= pVulkanShader->GetStageFlag();
			}
			else
			{
				bool bUniformValid = false;

				switch (StageUniform.Type)
				{
				case EShaderUniformType::Buffer:
				case EShaderUniformType::Texture:
					bUniformValid = true;
					break;
				default:
					break;
				}

				if (bUniformValid)
					mProgramUniforms.push_back(FVulkanUniformInfo(StageUniform, pVulkanShader->GetStageFlag()));
			}
		}
	}

	REV_CORE_ASSERT(mProgramUniforms.size() <= REV_VK_MAX_SHADER_UNIFORMS);
}

}


