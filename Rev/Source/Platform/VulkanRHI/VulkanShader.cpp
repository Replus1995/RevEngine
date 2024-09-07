#include "VulkanShader.h"
#include "VulkanCore.h"
#include "Core/VulkanPipeline.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanShader::FVulkanShader(ERHIShaderStage InStage, const FBuffer& InCompiledData)
	: FRHIShader(InStage)
{
	VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
	ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderModuleCreateInfo.codeSize = InCompiledData.Size();
	ShaderModuleCreateInfo.pCode = InCompiledData.DataAs<uint32_t>();

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
	case ERHIShaderStage::Fragment:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case ERHIShaderStage::TessControl:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case ERHIShaderStage::TessEval:
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
}

FVulkanShaderProgram::~FVulkanShaderProgram()
{
}

void FVulkanShaderProgram::FlushPipeline()
{
	if(!mPipelineStateDirty)
		return;
	mPipelineStateDirty = false;

	if(mPipeline)
		vkDestroyPipeline(FVulkanCore::GetDevice(), mPipeline, nullptr);

    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages = MakeShaderStageInfo(mShaders);
    VkPipelineVertexInputStateCreateInfo VertexInputState{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO }; //TODO
    VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = FVulkanGraphicsPipeline::MakeInputAssemblyStateInfo(PipelineState);
    //VkPipelineTessellationStateCreateInfo
    VkPipelineViewportStateCreateInfo ViewportState = FVulkanGraphicsPipeline::MakeViewportStateInfo(PipelineState);
    VkPipelineRasterizationStateCreateInfo RasterizationState = FVulkanGraphicsPipeline::MakeRasterizationStateInfo(PipelineState);
    //VkPipelineMultisampleStateCreateInfo
    VkPipelineDepthStencilStateCreateInfo DepthStencilState = FVulkanGraphicsPipeline::MakeDepthStencilStateInfo(PipelineState);
    VkPipelineColorBlendStateCreateInfo ColorBlendState = FVulkanGraphicsPipeline::MakeColorBlendStateInfo(PipelineState);

    // build the actual pipeline
    // we now use all of the info structs we have been writing into into this one
    // to create the pipeline
    VkGraphicsPipelineCreateInfo PipelineInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    // connect the renderInfo to the pNext extension mechanism
    PipelineInfo.pNext = &RenderInfo;

    PipelineInfo.stageCount = (uint32_t)ShaderStages.size();
    PipelineInfo.pStages = ShaderStages.data();
    PipelineInfo.pVertexInputState = &VertexInputInfo;
    PipelineInfo.pInputAssemblyState = &InputAssembly;
    PipelineInfo.pViewportState = &ViewportState;
    PipelineInfo.pRasterizationState = &Rasterizer;
    PipelineInfo.pMultisampleState = &Multisampling;
    PipelineInfo.pColorBlendState = &ColorBlendState;
    PipelineInfo.pDepthStencilState = &DepthStencil;
    PipelineInfo.layout = PipelineLayout;

    VkDynamicState DynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo DynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    DynamicInfo.pDynamicStates = &DynamicStates[0];
    DynamicInfo.dynamicStateCount = 2;

    PipelineInfo.pDynamicState = &DynamicInfo;

    // its easy to error out on create graphics pipeline, so we handle it a bit
   // better than the common VK_CHECK case
    VkPipeline NewPipeline;
    REV_VK_CHECK_THROW(vkCreateGraphicsPipelines(InDevice, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &NewPipeline), "failed to create pipeline");
    return NewPipeline;

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

}


