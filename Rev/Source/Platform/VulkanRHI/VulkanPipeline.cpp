#include "VulkanPipeline.h"
#include "VulkanCore.h"
#include "VulkanRenderPass.h"
#include "VulkanPrimitive.h"
#include "Core/VulkanDefines.h"
#include "Core/VulkanEnum.h"

namespace Rev
{

FVulkanPipeline::FVulkanPipeline()
{
}

FVulkanPipeline::~FVulkanPipeline()
{
    Release();
    ReleaseLayout();
}

void FVulkanPipeline::BuildLayout(const std::vector<VkDescriptorSetLayoutBinding>& InBindingInfo)
{
    VkDescriptorSetLayoutCreateInfo DescSetLayoutCreateInfo{};
    DescSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    DescSetLayoutCreateInfo.pNext = NULL;
    DescSetLayoutCreateInfo.flags = 0;
    DescSetLayoutCreateInfo.bindingCount = (uint32_t)InBindingInfo.size();
    DescSetLayoutCreateInfo.pBindings = InBindingInfo.data();

    REV_VK_CHECK_THROW(vkCreateDescriptorSetLayout(FVulkanCore::GetDevice(), &DescSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout), "failed to create descriptor set layout");

    VkPipelineLayoutCreateInfo GraphicsLayoutCreateInfo{};
    GraphicsLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    GraphicsLayoutCreateInfo.setLayoutCount = 1;
    GraphicsLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;
    GraphicsLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
    GraphicsLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

    REV_VK_CHECK_THROW(vkCreatePipelineLayout(FVulkanCore::GetDevice(), &GraphicsLayoutCreateInfo, nullptr, &mPipelineLayout), "failed to create pipeline layout");
}

void FVulkanPipeline::Build(const FRHIGraphicsPipelineState& InState, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, const FVulkanRenderPass* RenderPass, const FVulkanPrimitive* InPrimitive)
{
    if(!mPipelineLayout)
        return;
    Release();
    FVulkanGraphicsPipelineBuilder Builder(InState);
    mPipeline = Builder.Build(FVulkanCore::GetDevice(), mPipelineLayout, InShaderStageInfo, RenderPass, InPrimitive);
    mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void FVulkanPipeline::ReleaseLayout()
{
    if (mPipelineLayout)
        vkDestroyPipelineLayout(FVulkanCore::GetDevice(), mPipelineLayout, nullptr);
    if (mDescriptorSetLayout)
        vkDestroyDescriptorSetLayout(FVulkanCore::GetDevice(), mDescriptorSetLayout, nullptr);
}

void FVulkanPipeline::Release()
{
    if (mPipeline)
        vkDestroyPipeline(FVulkanCore::GetDevice(), mPipeline, nullptr);
}

FVulkanGraphicsPipelineBuilder::FVulkanGraphicsPipelineBuilder(const FRHIGraphicsPipelineState& InState)
    : mState(InState)
{
}

FVulkanGraphicsPipelineBuilder::~FVulkanGraphicsPipelineBuilder()
{
}

VkPipeline FVulkanGraphicsPipelineBuilder::Build(VkDevice InDevice, VkPipelineLayout InLayout, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, const FVulkanRenderPass* RenderPass, const FVulkanPrimitive* InPrimitive)
{
    const FRenderPassDesc& RenderPassDesc = RenderPass->GetDesc();
    uint32 ColorAttachmentCount = RenderPassDesc.NumColorAttachments;

    VkPipelineColorBlendAttachmentState AttachemntState{};
    AttachemntState.blendEnable = mState.ColorBlendAttachmentState.BlendEnable;
    AttachemntState.srcColorBlendFactor = FVulkanEnum::Translate(mState.ColorBlendAttachmentState.SrcColorBlendFactor);
    AttachemntState.dstColorBlendFactor = FVulkanEnum::Translate(mState.ColorBlendAttachmentState.DstColorBlendFactor);
    AttachemntState.colorBlendOp = FVulkanEnum::Translate(mState.ColorBlendAttachmentState.ColorBlendOp);
    AttachemntState.srcAlphaBlendFactor = FVulkanEnum::Translate(mState.ColorBlendAttachmentState.SrcAlphaBlendFactor);
    AttachemntState.dstAlphaBlendFactor = FVulkanEnum::Translate(mState.ColorBlendAttachmentState.DstAlphaBlendFactor);
    AttachemntState.alphaBlendOp = FVulkanEnum::Translate(mState.ColorBlendAttachmentState.AlphaBlendOp);
    AttachemntState.colorWriteMask = FVulkanEnum::Translate(mState.ColorBlendAttachmentState.ColorWriteMask);

    std::vector<VkPipelineColorBlendAttachmentState> AttachemntStateVec;
    AttachemntStateVec.reserve(ColorAttachmentCount);
    for (uint32 i = 0; i < ColorAttachmentCount; i++)
    {
        AttachemntStateVec.push_back(AttachemntState);
    }

    std::vector<VkDynamicState> DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    //std::vector<VkDynamicState> DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY };

    std::vector<VkFormat> ColorFormats;
    ColorFormats.reserve(ColorAttachmentCount);
    for (uint32 i = 0; i < ColorAttachmentCount; i++)
    {
        ColorFormats.push_back((VkFormat)GPixelFormats[RenderPassDesc.ColorAttachments[i].Format].PlatformFormat);
    }
    VkFormat DepthFormat = (VkFormat)GPixelFormats[RenderPassDesc.DepthStencilAttchment.Format].PlatformFormat;
    
    VkPipelineVertexInputStateCreateInfo VertexInputState{};
    VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputState.vertexBindingDescriptionCount = (uint32_t)InPrimitive->GetBindingDescs().size();
    VertexInputState.pVertexBindingDescriptions = InPrimitive->GetBindingDescs().data();
    VertexInputState.vertexAttributeDescriptionCount = (uint32_t)InPrimitive->GetAttributeDescs().size();
    VertexInputState.pVertexAttributeDescriptions = InPrimitive->GetAttributeDescs().data();


    VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = MakeInputAssemblyStateInfo();
    VkPipelineTessellationStateCreateInfo TessellationState = MakeTessellationStateInfo();
    VkPipelineViewportStateCreateInfo ViewportState = MakeViewportStateInfo();
    VkPipelineRasterizationStateCreateInfo RasterizationState = MakeRasterizationStateInfo();
    VkPipelineMultisampleStateCreateInfo MultisampleState = MakeMultisampleStateInfo();
    VkPipelineDepthStencilStateCreateInfo DepthStencilState = MakeDepthStencilStateInfo();
    VkPipelineColorBlendStateCreateInfo ColorBlendState = MakeColorBlendStateInfo(AttachemntStateVec);
    VkPipelineDynamicStateCreateInfo DynamicState = MakeDynamicStateInfo(DynamicStates);
    VkPipelineRenderingCreateInfo Rendering = MakeRenderingInfo(ColorFormats, DepthFormat);

    // build the actual pipeline
    // we now use all of the info structs we have been writing into into this one
    // to create the pipeline
    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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
    PipelineInfo.layout = InLayout;
    PipelineInfo.renderPass = (VkRenderPass)RenderPass->GetNativeHandle();
    PipelineInfo.subpass = 0;


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
    StateInfo.viewportCount = 1;
    StateInfo.scissorCount = 1;
    return StateInfo;
}

VkPipelineRasterizationStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeRasterizationStateInfo() const
{
    VkPipelineRasterizationStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    StateInfo.depthClampEnable = mState.DepthClampEnable;
    StateInfo.rasterizerDiscardEnable = VK_FALSE;
    StateInfo.polygonMode = FVulkanEnum::Translate(mState.PolygonMode);
    StateInfo.cullMode = FVulkanEnum::Translate(mState.CullMode);
    StateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    StateInfo.depthBiasEnable = mState.DepthBiasEnable;
    StateInfo.depthBiasConstantFactor = mState.DepthBiasConstantFactor;
    StateInfo.depthBiasClamp = mState.DepthBiasClamp;
    StateInfo.depthBiasSlopeFactor = mState.DepthBiasSlopeFactor;
    //StateInfo.lineWidth = mState.LineWidth;
    StateInfo.lineWidth = 1.0;
    return StateInfo;
}

VkPipelineMultisampleStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeMultisampleStateInfo() const
{
    VkPipelineMultisampleStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    StateInfo.sampleShadingEnable = VK_FALSE;
    StateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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

VkPipelineColorBlendStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeColorBlendStateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& InAttachmentStates) const
{
    VkPipelineColorBlendStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    StateInfo.logicOpEnable = VK_FALSE;
    StateInfo.logicOp = VK_LOGIC_OP_COPY;
    StateInfo.pAttachments = InAttachmentStates.data();
    StateInfo.attachmentCount = (uint32_t)InAttachmentStates.size();

    for (size_t i = 0; i < 4; i++)
    {
        StateInfo.blendConstants[i] = mState.BlendConstants[i];
    }

    return StateInfo;
}

VkPipelineDynamicStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeDynamicStateInfo(const std::vector<VkDynamicState>& InDynaimcStates) const
{
    VkPipelineDynamicStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    StateInfo.pDynamicStates = InDynaimcStates.data();
    StateInfo.dynamicStateCount = (uint32_t)InDynaimcStates.size();
    return StateInfo;
}

VkPipelineRenderingCreateInfo FVulkanGraphicsPipelineBuilder::MakeRenderingInfo(const std::vector<VkFormat>& InColorFomats, VkFormat InDepthFormat) const
{
   VkPipelineRenderingCreateInfo StateInfo{};
   StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
   StateInfo.pColorAttachmentFormats = InColorFomats.data();
   StateInfo.colorAttachmentCount = (uint32_t)InColorFomats.size();
   StateInfo.depthAttachmentFormat = InDepthFormat;
   StateInfo.stencilAttachmentFormat = InDepthFormat;
   return StateInfo;
}

}