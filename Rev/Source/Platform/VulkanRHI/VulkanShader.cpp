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

VkShaderStageFlagBits FVulkanShader::TranslateShaderStage(ERHIShaderStage InStage)
{
	switch (InStage)
	{
	case ERHIShaderStage::Vertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case ERHIShaderStage::Pixel:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case ERHIShaderStage::Hull:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case ERHIShaderStage::Domain:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case ERHIShaderStage::Geometry:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	case ERHIShaderStage::Compute:
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

std::vector<VkPipelineShaderStageCreateInfo> FVulkanShaderProgram::GenShaderStageInfo() const
{
    std::vector<VkPipelineShaderStageCreateInfo> StageInfoVec;
    for (uint8 i = (uint8)ERHIShaderStage::Vertex; i < (uint8)ERHIShaderStage::Compute; i++)
    {
        const auto& pShader = mShaders[(ERHIShaderStage)i];
        if(!pShader)
            continue;
        VkPipelineShaderStageCreateInfo StageInfo{};
        StageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        StageInfo.pNext = nullptr;
        StageInfo.stage = FVulkanShader::TranslateShaderStage((ERHIShaderStage)i);
        StageInfo.module = (VkShaderModule)pShader->GetNativeHandle();
        StageInfo.pName = "main";

        StageInfoVec.emplace_back(std::move(StageInfo));
    }
    return StageInfoVec;
}

std::vector<VkDescriptorSetLayoutBinding> FVulkanShaderProgram::GenLayoutBindings() const
{
	std::vector<VkDescriptorSetLayoutBinding> AllBindings;
	for (const FVulkanUniformInfo& Uniform : mProgramUniforms)
	{
		switch (Uniform.Type)
		{
		case ERHIUniformType::Buffer:
		{
			VkDescriptorSetLayoutBinding Binding{};
			Binding.stageFlags = Uniform.StageFlags;
			Binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			Binding.binding = Uniform.Binding;
			Binding.pImmutableSamplers = NULL;
			Binding.descriptorCount = 1;

			AllBindings.push_back(Binding);
		}
		break;
		default:
			break;
		}
	}

	return AllBindings;
}

void FVulkanShaderProgram::UpdateProgramUniforms()
{
	mProgramUniforms.clear();

	for (uint8 i = (uint8)ERHIShaderStage::Vertex; i < (uint8)ERHIShaderStage::Compute; i++)
	{
		const auto& pShader = mShaders[(ERHIShaderStage)i];
		if (!pShader)
			continue;
		FVulkanShader* pVulkanShader = static_cast<FVulkanShader*>(pShader.get());
		const std::vector<FRHIShaderUniform>& StageUniforms = pVulkanShader->GetStageUniforms();
		for (const FRHIShaderUniform& StageUniform : StageUniforms)
		{

			if (auto iter = std::find_if(mProgramUniforms.begin(), mProgramUniforms.end(), [&StageUniform](const FVulkanUniformInfo& Elem) { return Elem.Binding == StageUniform.Binding; });
				iter != mProgramUniforms.end())
			{
				//REV_CORE_ASSERT(iter->descriptorType == Binding.descriptorType);
				iter->StageFlags |= pVulkanShader->GetStageFlag();
			}
			else
			{
				bool bUniformValid = false;

				switch (StageUniform.Type)
				{
				case ERHIUniformType::Buffer:
				{
					bUniformValid = true;
				}
				break;

				default:
					break;
				}

				if (bUniformValid)
					mProgramUniforms.push_back(FVulkanUniformInfo(StageUniform, pVulkanShader->GetStageFlag()));
			}
		}
	}

	REV_CORE_ASSERT(mProgramUniforms.size() <= REV_VK_MAX_DESCRIPTORSETS);
}

}


