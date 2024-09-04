#include "VulkanPipeline.h"
#include "VulkanDefines.h"

namespace Rev
{

FVulkanPipelineBuilder::FVulkanPipelineBuilder()
{
	Clear();
}

VkPipeline FVulkanPipelineBuilder::Build(VkDevice InDevice)
{
    // make viewport state from our stored viewport and scissor.
    // at the moment we wont support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo ViewportState = {};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.pNext = nullptr;
    ViewportState.viewportCount = 1;
    ViewportState.scissorCount = 1;

    // setup dummy color blending. We arent using transparent objects yet
    // the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo ColorBlendState = {};
    ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlendState.pNext = nullptr;

    ColorBlendState.logicOpEnable = VK_FALSE;
    ColorBlendState.logicOp = VK_LOGIC_OP_COPY;
    ColorBlendState.attachmentCount = 1;
    ColorBlendState.pAttachments = &ColorBlendAttachment;

    // completely clear VertexInputStateCreateInfo, as we have no need for it
    VkPipelineVertexInputStateCreateInfo VertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

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

void FVulkanPipelineBuilder::Clear()
{
    ShaderStages.clear();
    InputAssembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    Rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO  };
    ColorBlendAttachment = {};
    Multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    PipelineLayout = {};
    DepthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    RenderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
}

}