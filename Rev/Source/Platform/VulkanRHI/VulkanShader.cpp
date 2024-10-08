#include "VulkanShader.h"
#include "VulkanCore.h"
#include "VulkanRenderTarget.h"
#include "VulkanPrimitive.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanShader::FVulkanShader(const FShadercCompiledData& InCompiledData)
	: FRHIShader(InCompiledData.Stage)
	, mDebugName(InCompiledData.Name)
	, mStageFlags(TranslateShaderStage(InCompiledData.Stage))
{
	VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
	ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderModuleCreateInfo.codeSize = InCompiledData.Binary.Size();
	ShaderModuleCreateInfo.pCode = InCompiledData.Binary.DataAs<uint32_t>();

	REV_VK_CHECK_THROW(vkCreateShaderModule(FVulkanCore::GetDevice(), &ShaderModuleCreateInfo, nullptr, &mModule), "[FVkShader] Failed to create shader module!");
}

FVulkanShader::~FVulkanShader()
{
	vkDestroyShaderModule(FVulkanCore::GetDevice(), mModule, nullptr);
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

void FVulkanShader::InitBindings(const FShadercCompiledData& InCompiledData)
{
	uint16 Index = 0;
	for (const FShadercUniform& Uniform : InCompiledData.Uniforms)
	{
		switch (Uniform.Type)
		{
		case SUT_Buffer:
		{
			VkDescriptorSetLayoutBinding& Binding = mBindings[Index];
			Binding.stageFlags = mStageFlags;
			Binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			Binding.binding = Uniform.Binding;
			Binding.pImmutableSamplers = NULL;
			Binding.descriptorCount = 1;

			Index++;
		}
		break;
		
		default:
			break;
		}
	}
	mNumBindings = Index;
}

FVulkanShaderProgram::FVulkanShaderProgram(const std::string& InName, const FRHIGraphicsShaders& InShaders)
	: FRHIShaderProgram(InName)
    , mShaders(InShaders)
{
}

FVulkanShaderProgram::~FVulkanShaderProgram()
{
}

void FVulkanShaderProgram::PrepareDraw(const FVulkanRenderTarget* RenderTarget, const FVulkanPrimitive* Primitive)
{
	if (!mPipelineStateDirty && mNumColorTargetsCache == RenderTarget->GetDesc().NumColorTargets && mIuputDescHashCache == Primitive->GetDescHash())
		return;
	mPipelineStateDirty = false;
	mNumColorTargetsCache = RenderTarget->GetDesc().NumColorTargets;
	mIuputDescHashCache = Primitive->GetDescHash();

	std::vector<VkPipelineShaderStageCreateInfo> ShaderStages = MakeShaderStageInfo(mShaders);
	std::vector<VkDescriptorSetLayoutBinding> LayoutBindings = MakeLayoutBindings(mShaders);
	mPipeline.Build(PipelineState, LayoutBindings, ShaderStages, RenderTarget, Primitive);
}

std::vector<VkPipelineShaderStageCreateInfo> FVulkanShaderProgram::MakeShaderStageInfo(const FRHIGraphicsShaders& InShaders)
{
    std::vector<VkPipelineShaderStageCreateInfo> StageInfoVec;
    for (uint8 i = 0; i < (uint8)ERHIShaderStage::Count; i++)
    {
        const auto& pShader = InShaders[(ERHIShaderStage)i];
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

std::vector<VkDescriptorSetLayoutBinding> FVulkanShaderProgram::MakeLayoutBindings(const FRHIGraphicsShaders& InShaders)
{
	std::vector<VkDescriptorSetLayoutBinding> AllBindings;
	for (uint8 i = 0; i < (uint8)ERHIShaderStage::Count; i++)
	{
		const auto& pShader = InShaders[(ERHIShaderStage)i];
		if (!pShader)
			continue;
		FVulkanShader* pVulkanShader = static_cast<FVulkanShader*>(pShader.get());
		for (uint16 j = 0; j < pVulkanShader->GetNumBindings(); j++)
		{
			const VkDescriptorSetLayoutBinding& Binding = pVulkanShader->GetBindings()[j];
			if (auto iter = std::find_if(AllBindings.begin(), AllBindings.end(), [Binding](const VkDescriptorSetLayoutBinding& Elem) { return Elem.binding == Binding.binding; });
				iter != AllBindings.end())
			{
				REV_CORE_ASSERT(iter->descriptorType == Binding.descriptorType);
				iter->stageFlags |= pVulkanShader->GetStageFlags();
			}
			else
			{
				AllBindings.push_back(Binding);
			}
		}
	}
	return AllBindings;
}

}


