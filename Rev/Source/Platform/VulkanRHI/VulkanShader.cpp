#include "VulkanShader.h"
#include "VulkanContext.h"
#include "VulkanDynamicRHI.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanShader::FVulkanShader(const FShadercCompiledData& InCompiledData)
	: FRHIShader(InCompiledData.Stage)
	, mName(InCompiledData.Name)
	, mStageFlag(TranslateShaderStage(InCompiledData.Stage))
{
	REV_CORE_ASSERT(!mName.empty());

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
	case SS_Vertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case SS_Pixel:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case SS_Hull:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case SS_Domain:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case SS_Geometry:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	case SS_Compute:
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
    for (uint8 i = SS_Vertex; i < SS_NumGraphics; i++)
    {
        const auto& pShader = mShaders[i];
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
			Binding.descriptorCount = Uniform.Num;
			Binding.binding = Uniform.Binding;
			Binding.pImmutableSamplers = NULL;
		}
		break;
		case EShaderUniformType::Texture:
		{
			VkDescriptorSetLayoutBinding& Binding = OutBindings[BindingsCount++];
			Binding.stageFlags = Uniform.StageFlags;
			Binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			Binding.descriptorCount = Uniform.Num;
			Binding.binding = Uniform.Binding;
			Binding.pImmutableSamplers = NULL;

			if (Uniform.SamplerBinding >= 0)
			{
				VkDescriptorSetLayoutBinding& Binding = OutBindings[BindingsCount++];
				Binding.stageFlags = Uniform.StageFlags;
				Binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				Binding.descriptorCount = 1;
				Binding.binding = Uniform.SamplerBinding;
				Binding.pImmutableSamplers = NULL;
			}
		}
		break;
		case EShaderUniformType::SamplerState:
		{
			VkDescriptorSetLayoutBinding& Binding = OutBindings[BindingsCount++];
			Binding = {};
			Binding.stageFlags = Uniform.StageFlags;
			Binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			Binding.descriptorCount = 1;
			Binding.binding = Uniform.Binding;
			Binding.pImmutableSamplers = NULL;
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

	for (uint8 i = SS_Vertex; i < SS_NumGraphics; i++)
	{
		const auto& pShader = mShaders[i];
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
				case EShaderUniformType::SamplerState:
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


