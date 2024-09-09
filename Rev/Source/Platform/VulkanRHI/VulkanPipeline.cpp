#include "VulkanPipeline.h"
#include "Core/VulkanDefines.h"
#include "Core/VulkanEnum.h"

namespace Rev
{

FVulkanGraphicsPipelineBuilder::FVulkanGraphicsPipelineBuilder(const FRHIGraphicsPipelineState& InState)
    : mState(InState)
{
    mAttachemntStateVec.reserve(InState.ColorBlendAttachmentStates.size());
    for (const auto& SrcAttachemntState : InState.ColorBlendAttachmentStates)
    {
        VkPipelineColorBlendAttachmentState AttachemntState{};
        AttachemntState.blendEnable = SrcAttachemntState.BlendEnable;
        AttachemntState.srcColorBlendFactor = FVulkanEnum::Translate(SrcAttachemntState.SrcColorBlendFactor);
        AttachemntState.dstColorBlendFactor = FVulkanEnum::Translate(SrcAttachemntState.DstColorBlendFactor);
        AttachemntState.colorBlendOp = FVulkanEnum::Translate(SrcAttachemntState.ColorBlendOp);
        AttachemntState.srcAlphaBlendFactor = FVulkanEnum::Translate(SrcAttachemntState.SrcAlphaBlendFactor);
        AttachemntState.dstAlphaBlendFactor = FVulkanEnum::Translate(SrcAttachemntState.DstAlphaBlendFactor);
        AttachemntState.alphaBlendOp = FVulkanEnum::Translate(SrcAttachemntState.AlphaBlendOp);
        AttachemntState.colorWriteMask = FVulkanEnum::Translate(SrcAttachemntState.ColorWriteMask);
        mAttachemntStateVec.emplace_back(std::move(AttachemntState));
    }
}

FVulkanGraphicsPipelineBuilder::~FVulkanGraphicsPipelineBuilder()
{
}

VkPipeline FVulkanGraphicsPipelineBuilder::Build(VkDevice InDevice, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo) const
{
    VkPipelineVertexInputStateCreateInfo VertexInputState{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO }; //TODO
    VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = MakeInputAssemblyStateInfo();
    VkPipelineTessellationStateCreateInfo TessellationState = MakeTessellationStateInfo();
    VkPipelineViewportStateCreateInfo ViewportState = MakeViewportStateInfo();
    VkPipelineRasterizationStateCreateInfo RasterizationState = MakeRasterizationStateInfo();
    VkPipelineMultisampleStateCreateInfo MultisampleState = MakeMultisampleStateInfo();
    VkPipelineDepthStencilStateCreateInfo DepthStencilState = MakeDepthStencilStateInfo();
    VkPipelineColorBlendStateCreateInfo ColorBlendState = MakeColorBlendStateInfo();
    VkPipelineDynamicStateCreateInfo DynamicState = MakeDynamicStateInfo();
    VkPipelineRenderingCreateInfo Rendering = MakeRenderingInfo();

    VkPipelineLayout Layout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo GraphicsLayout{};
    GraphicsLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    GraphicsLayout.pNext = nullptr;
    GraphicsLayout.pSetLayouts = &_drawImageDescriptorLayout;
    GraphicsLayout.setLayoutCount = 1;

    REV_VK_CHECK_THROW(vkCreatePipelineLayout(InDevice, &GraphicsLayout, nullptr, &Layout), "failed to create pipeline layout");

    // build the actual pipeline
    // we now use all of the info structs we have been writing into into this one
    // to create the pipeline
    VkGraphicsPipelineCreateInfo PipelineInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    // connect the renderInfo to the pNext extension mechanism
    PipelineInfo.pNext = &Rendering;

    PipelineInfo.stageCount = (uint32_t)InShaderStageInfo.size();
    PipelineInfo.pStages = InShaderStageInfo.data();
    PipelineInfo.pVertexInputState = &VertexInputState;
    PipelineInfo.pInputAssemblyState = &InputAssemblyState;
    PipelineInfo.pViewportState = &ViewportState;
    PipelineInfo.pRasterizationState = &RasterizationState;
    PipelineInfo.pMultisampleState = &MultisampleState;
    PipelineInfo.pDepthStencilState = &DepthStencilState;
    PipelineInfo.pColorBlendState = &ColorBlendState;
    PipelineInfo.pDynamicState = &DynamicState;
    PipelineInfo.layout = Layout;

    // its easy to error out on create graphics pipeline, so we handle it a bit
   // better than the common VK_CHECK case
    VkPipeline NewPipeline;
    REV_VK_CHECK_THROW(vkCreateGraphicsPipelines(InDevice, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &NewPipeline), "failed to create pipeline");
    return NewPipeline;
}

VkPipelineInputAssemblyStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeInputAssemblyStateInfo() const
{
    VkPipelineInputAssemblyStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    StateInfo.topology = FVulkanEnum::Translate(mState.PrimitiveTopology);
    StateInfo.primitiveRestartEnable = VK_FALSE;
    return StateInfo;
}

VkPipelineTessellationStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeTessellationStateInfo() const
{
   VkPipelineTessellationStateCreateInfo StateInfo{};
   StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
   StateInfo.patchControlPoints = 0;
   return StateInfo;
}

VkPipelineViewportStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeViewportStateInfo() const
{
    // make viewport state from our stored viewport and scissor.
    // at the moment we wont support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    StateInfo.pNext = nullptr;
    StateInfo.viewportCount = 1;
    StateInfo.scissorCount = 1;
    return StateInfo;
}

VkPipelineRasterizationStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeRasterizationStateInfo() const
{
    VkPipelineRasterizationStateCreateInfo StateInfo{};
    StateInfo.depthClampEnable = mState.DepthClampEnable;
    StateInfo.rasterizerDiscardEnable = VK_FALSE;
    StateInfo.polygonMode = FVulkanEnum::Translate(mState.PolygonMode);
    StateInfo.cullMode = FVulkanEnum::Translate(mState.CullMode);
    StateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    StateInfo.depthBiasEnable = mState.DepthBiasEnable;
    StateInfo.depthBiasConstantFactor = mState.DepthBiasConstantFactor;
    StateInfo.depthBiasClamp = mState.DepthBiasClamp;
    StateInfo.depthBiasSlopeFactor = mState.DepthBiasSlopeFactor;
    StateInfo.lineWidth = mState.LineWidth;
    return StateInfo;
}

VkPipelineMultisampleStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeMultisampleStateInfo() const
{
    VkPipelineMultisampleStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    StateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    StateInfo.sampleShadingEnable = VK_FALSE;
    StateInfo.minSampleShading = 1.0f;
    StateInfo.pSampleMask = nullptr;
    StateInfo.alphaToCoverageEnable = VK_FALSE;
    StateInfo.alphaToOneEnable = VK_FALSE;
    return StateInfo;
}

VkPipelineDepthStencilStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeDepthStencilStateInfo() const
{
    VkPipelineDepthStencilStateCreateInfo StateInfo{};
    StateInfo.depthTestEnable = mState.DepthTestEnable;
    StateInfo.depthWriteEnable = mState.DepthWriteEnable;
    StateInfo.depthCompareOp = FVulkanEnum::Translate(mState.DepthCompareOp);
    //TODO: fill stencil state options
    StateInfo.depthBoundsTestEnable = VK_FALSE;
    StateInfo.stencilTestEnable = VK_FALSE;
    StateInfo.front = {};
    StateInfo.back = {};
    StateInfo.minDepthBounds = 0.f;
    StateInfo.maxDepthBounds = 1.f;

    return StateInfo;
}

VkPipelineColorBlendStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeColorBlendStateInfo() const
{
    // setup dummy color blending. We arent using transparent objects yet
    // the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo StateInfo = {};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    StateInfo.pNext = nullptr;
    StateInfo.logicOpEnable = VK_FALSE;
    StateInfo.logicOp = VK_LOGIC_OP_COPY;
    StateInfo.attachmentCount = (uint32_t)mAttachemntStateVec.size();
    StateInfo.pAttachments = mAttachemntStateVec.data();

    for (size_t i = 0; i < 4; i++)
    {
        StateInfo.blendConstants[i] = mState.BlendConstants[i];
    }

    return StateInfo;
}

VkPipelineDynamicStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeDynamicStateInfo() const
{
    VkPipelineDynamicStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    StateInfo.pDynamicStates = &mDynamicStates[0];
    StateInfo.dynamicStateCount = 2;
    return StateInfo;
}

VkPipelineRenderingCreateInfo FVulkanGraphicsPipelineBuilder::MakeRenderingInfo() const
{
    //TODO: generate rendering info from rendertarget
    VkFormat ColorFormat, DepthFormat, StencilFomat;

   VkPipelineRenderingCreateInfo StateInfo{};
   StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
   StateInfo.pColorAttachmentFormats = &ColorFormat;
   StateInfo.colorAttachmentCount = 1;
   StateInfo.depthAttachmentFormat = DepthFormat;
   StateInfo.stencilAttachmentFormat = StencilFomat;
   return StateInfo;
}

}