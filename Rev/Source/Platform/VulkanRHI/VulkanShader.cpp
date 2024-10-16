#include "VulkanShader.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanPrimitive.h"
#include "VulkanUniform.h"
#include "VulkanDynamicRHI.h"
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
	std::vector<VkDescriptorSetLayoutBinding> LayoutBindings = MakeLayoutBindings(mShaders);
	mPipelineLayout.Build(LayoutBindings);
}

FVulkanShaderProgram::~FVulkanShaderProgram()
{
}

void FVulkanShaderProgram::PrepareDraw(FVulkanContext* Context, const FVulkanRenderPass* RenderPass, const FVulkanPrimitive* Primitive)
{
	if (mGraphicsStateDirty)
	{
		mGraphicsStateDirty = false;
		mPipelineCache.Clear();
		mPipeline.reset();
	}

	FVulkanPipelineKey NewKey = { (VkRenderPass)RenderPass->GetNativeHandle(), Primitive->GetDescHash() };

	if (!mPipeline || mPipelineKey != NewKey)
	{
		mPipelineKey = NewKey;
		if (Ref<FVulkanPipeline> CachedPipeline = mPipelineCache.Find(NewKey); CachedPipeline != nullptr)
		{
			mPipeline = CachedPipeline;
		}
		else
		{
			mPipeline = CreateRef<FVulkanPipeline>();
			std::vector<VkPipelineShaderStageCreateInfo> ShaderStages = MakeShaderStageInfo(mShaders);
			mPipeline->Build(mPipelineLayout, GraphicsState, ShaderStages, RenderPass, Primitive);
			mPipelineCache.Add(NewKey, mPipeline);
		}

	}

	VkDescriptorSet DescSet = GetDescriptorSet(Context);
	vkCmdBindDescriptorSets(Context->GetActiveCmdBuffer(), mPipeline->GetPipelineBindPoint(), mPipelineLayout.GetPipelineLayout(), 0, 1, &DescSet, 0, nullptr);
	vkCmdBindPipeline(Context->GetActiveCmdBuffer(), mPipeline->GetPipelineBindPoint(), mPipeline->GetPipeline());
}

std::vector<VkPipelineShaderStageCreateInfo> FVulkanShaderProgram::MakeShaderStageInfo(const FRHIGraphicsShaders& InShaders)
{
    std::vector<VkPipelineShaderStageCreateInfo> StageInfoVec;
    for (uint8 i = (uint8)ERHIShaderStage::Vertex; i < (uint8)ERHIShaderStage::Compute; i++)
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
	mProgramUniforms.clear();

	std::vector<VkDescriptorSetLayoutBinding> AllBindings;
	for (uint8 i = (uint8)ERHIShaderStage::Vertex; i < (uint8)ERHIShaderStage::Compute; i++)
	{
		const auto& pShader = InShaders[(ERHIShaderStage)i];
		if (!pShader)
			continue;
		FVulkanShader* pVulkanShader = static_cast<FVulkanShader*>(pShader.get());
		const std::vector<FRHIUniformInfo>& StageUniforms = pVulkanShader->GetStageUniforms();
		for (const FRHIUniformInfo& StageUniform : StageUniforms)
		{
			
			if (auto iter = std::find_if(AllBindings.begin(), AllBindings.end(), [&StageUniform](const VkDescriptorSetLayoutBinding& Elem) { return Elem.binding == StageUniform.Binding; });
				iter != AllBindings.end())
			{
				//REV_CORE_ASSERT(iter->descriptorType == Binding.descriptorType);
				iter->stageFlags |= pVulkanShader->GetStageFlags();
			}
			else
			{
				bool bUniformValid = false;

				switch (StageUniform.Type)
				{
				case ERHIUniformType::Buffer:
				{
					VkDescriptorSetLayoutBinding Binding{};
					Binding.stageFlags = pVulkanShader->GetStageFlags();
					Binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					Binding.binding = StageUniform.Binding;
					Binding.pImmutableSamplers = NULL;
					Binding.descriptorCount = 1;

					AllBindings.push_back(Binding);
					bUniformValid = true;
				}
				break;

				default:
					break;
				}

				if(bUniformValid)
					mProgramUniforms.push_back(StageUniform);
			}
		}
	}

	REV_CORE_ASSERT(mProgramUniforms.size() <= REV_VK_MAX_DESCRIPTORSETS);

	return AllBindings;
}

VkDescriptorSet FVulkanShaderProgram::GetDescriptorSet(FVulkanContext* Context)
{
	VkDescriptorSetLayout DescLayout = mPipelineLayout.GetDescriptorSetLayout();

	VkDescriptorSet DescSet;
	VkDescriptorSetAllocateInfo DescAllocateInfo{};
	DescAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescAllocateInfo.pNext = NULL;
	DescAllocateInfo.descriptorPool = Context->GetActiveDescriptorPool().Pool;
	DescAllocateInfo.descriptorSetCount = 1;
	DescAllocateInfo.pSetLayouts = &DescLayout;

	REV_VK_CHECK(vkAllocateDescriptorSets(FVulkanDynamicRHI::GetDevice(), &DescAllocateInfo, &DescSet));


	VkDescriptorBufferInfo BufferInfos[REV_VK_MAX_SHADER_UNIFORM_BUFFERS] = {};
	VkWriteDescriptorSet Writes[REV_VK_MAX_DESCRIPTORSETS] = {};
	uint32 WriteCount = 0;
	uint32_t BufferCount = 0;

	for (const FRHIUniformInfo& Uniform : mProgramUniforms)
	{
		uint32 BindingIdx = Uniform.Binding;
		FVulkanUniformBuffer* UniformBuffer = Context->FindUniformBuffer(BindingIdx);
		if(!UniformBuffer)
			continue;

		BufferInfos[BufferCount].buffer = (VkBuffer)UniformBuffer->GetNativeHandle();
		BufferInfos[BufferCount].offset = 0;
		BufferInfos[BufferCount].range = UniformBuffer->GetSize();

		Writes[WriteCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		Writes[WriteCount].pNext = NULL;
		Writes[WriteCount].dstSet = DescSet;
		Writes[WriteCount].dstBinding = BindingIdx;
		Writes[WriteCount].dstArrayElement = 0;
		Writes[WriteCount].descriptorCount = 1;
		Writes[WriteCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		Writes[WriteCount].pImageInfo = NULL;
		Writes[WriteCount].pBufferInfo = &BufferInfos[BufferCount];
		Writes[WriteCount].pTexelBufferView = NULL;

		++WriteCount;
		++BufferCount;
	}

	vkUpdateDescriptorSets(FVulkanDynamicRHI::GetDevice(), WriteCount, Writes, 0, NULL);

	return DescSet;
}

}


