#include "VulkanPipeline.h"
#include "VulkanDynamicRHI.h"
#include "VulkanRenderPass.h"
#include "VulkanPrimitive.h"
#include "Core/VulkanDefines.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanGraphicsPipelineBuilder::FVulkanGraphicsPipelineBuilder(const FRHIGraphicsState& InState)
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
    REV_CORE_ASSERT(ColorAttachmentCount <= RTA_MaxColorAttachments);

    VkPipelineColorBlendAttachmentState ColorBlendStateArr[RTA_MaxColorAttachments];
    for (uint32 i = 0; i < ColorAttachmentCount; i++)
    {
        ColorBlendStateArr[i].blendEnable = mState.ColorBlendStates[i].bEnable;
        ColorBlendStateArr[i].srcColorBlendFactor = FVulkanEnum::Translate(mState.ColorBlendStates[i].SrcColorFactor);
        ColorBlendStateArr[i].dstColorBlendFactor = FVulkanEnum::Translate(mState.ColorBlendStates[i].DstColorFactor);
        ColorBlendStateArr[i].colorBlendOp = FVulkanEnum::Translate(mState.ColorBlendStates[i].ColorOp);
        ColorBlendStateArr[i].srcAlphaBlendFactor = FVulkanEnum::Translate(mState.ColorBlendStates[i].SrcAlphaBlendFactor);
        ColorBlendStateArr[i].dstAlphaBlendFactor = FVulkanEnum::Translate(mState.ColorBlendStates[i].DstAlphaBlendFactor);
        ColorBlendStateArr[i].alphaBlendOp = FVulkanEnum::Translate(mState.ColorBlendStates[i].AlphaBlendOp);
        ColorBlendStateArr[i].colorWriteMask = FVulkanEnum::Translate(mState.ColorBlendStates[i].ColorWriteMask);
    }

   VkDynamicState DynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    //std::vector<VkDynamicState> DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY };

    VkFormat ColorFormats[RTA_MaxColorAttachments];
    for (uint32 i = 0; i < ColorAttachmentCount; i++)
    {
        ColorFormats[i] = (VkFormat)GPixelFormats[RenderPassDesc.ColorAttachments[i].Format].PlatformFormat;
    }
    VkFormat DepthFormat = (VkFormat)GPixelFormats[RenderPassDesc.DepthStencilAttchment.Format].PlatformFormat;
    
    VkPipelineVertexInputStateCreateInfo VertexInputState{};
    VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputState.vertexBindingDescriptionCount = (uint32_t)InPrimitive->GetBindingDescs().size();
    VertexInputState.pVertexBindingDescriptions = InPrimitive->GetBindingDescs().data();
    VertexInputState.vertexAttributeDescriptionCount = (uint32_t)InPrimitive->GetAttributeDescs().size();
    VertexInputState.pVertexAttributeDescriptions = InPrimitive->GetAttributeDescs().data();


    VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = MakeInputAssemblyStateInfo(FVulkanEnum::Translate(InPrimitive->GetTopology()));
    VkPipelineTessellationStateCreateInfo TessellationState = MakeTessellationStateInfo();
    VkPipelineViewportStateCreateInfo ViewportState = MakeViewportStateInfo();
    VkPipelineRasterizationStateCreateInfo RasterizationState = MakeRasterizationStateInfo();
    VkPipelineMultisampleStateCreateInfo MultisampleState = MakeMultisampleStateInfo();
    VkPipelineDepthStencilStateCreateInfo DepthStencilState = MakeDepthStencilStateInfo();
    VkPipelineColorBlendStateCreateInfo ColorBlendState = MakeColorBlendStateInfo(ColorBlendStateArr, ColorAttachmentCount);
    VkPipelineDynamicStateCreateInfo DynamicState = MakeDynamicStateInfo(DynamicStates, sizeof(DynamicStates) / sizeof(VkDynamicState));
    VkPipelineRenderingCreateInfo Rendering = MakeRenderingInfo(ColorFormats, ColorAttachmentCount, DepthFormat);

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

VkPipelineInputAssemblyStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeInputAssemblyStateInfo(VkPrimitiveTopology InTopology) const
{
    VkPipelineInputAssemblyStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    StateInfo.topology = InTopology;
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
    StateInfo.depthBiasClamp = mState.DepthBiasEnable ? mState.DepthBiasClamp : 0.0f;
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

VkPipelineColorBlendStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeColorBlendStateInfo(const VkPipelineColorBlendAttachmentState* InColorBlendStates, uint32 InNumColorBlendStates) const
{
    VkPipelineColorBlendStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    StateInfo.logicOpEnable = VK_FALSE;
    StateInfo.logicOp = VK_LOGIC_OP_COPY;
    StateInfo.pAttachments = InColorBlendStates;
    StateInfo.attachmentCount = InNumColorBlendStates;

    for (size_t i = 0; i < 4; i++)
    {
        StateInfo.blendConstants[i] = mState.BlendConstants[i];
    }

    return StateInfo;
}

VkPipelineDynamicStateCreateInfo FVulkanGraphicsPipelineBuilder::MakeDynamicStateInfo(const VkDynamicState* InDynaimcStates, uint32 InNumDynaimcStates) const
{
    VkPipelineDynamicStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    StateInfo.pDynamicStates = InDynaimcStates;
    StateInfo.dynamicStateCount = InNumDynaimcStates;
    return StateInfo;
}

VkPipelineRenderingCreateInfo FVulkanGraphicsPipelineBuilder::MakeRenderingInfo(const VkFormat* InColorFomats, uint32 InNumColorFormats, VkFormat InDepthFormat) const
{
   VkPipelineRenderingCreateInfo StateInfo{};
   StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
   StateInfo.pColorAttachmentFormats = InColorFomats;
   StateInfo.colorAttachmentCount = InNumColorFormats;
   StateInfo.depthAttachmentFormat = InDepthFormat;
   StateInfo.stencilAttachmentFormat = InDepthFormat;
   return StateInfo;
}

FVulkanPipelineLayout::FVulkanPipelineLayout()
{
}

FVulkanPipelineLayout::~FVulkanPipelineLayout()
{
    Release();
}

void FVulkanPipelineLayout::Build(const std::vector<VkDescriptorSetLayoutBinding>& InBindingInfo)
{
    VkDescriptorSetLayoutCreateInfo DescSetLayoutCreateInfo{};
    DescSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    DescSetLayoutCreateInfo.pNext = NULL;
    DescSetLayoutCreateInfo.flags = 0;
    DescSetLayoutCreateInfo.bindingCount = (uint32_t)InBindingInfo.size();
    DescSetLayoutCreateInfo.pBindings = InBindingInfo.data();

    REV_VK_CHECK_THROW(vkCreateDescriptorSetLayout(FVulkanDynamicRHI::GetDevice(), &DescSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout), "failed to create descriptor set layout");

    VkPipelineLayoutCreateInfo GraphicsLayoutCreateInfo{};
    GraphicsLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    GraphicsLayoutCreateInfo.setLayoutCount = 1;
    GraphicsLayoutCreateInfo.pSetLayouts = &mDescriptorSetLayout;
    GraphicsLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
    GraphicsLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

    REV_VK_CHECK_THROW(vkCreatePipelineLayout(FVulkanDynamicRHI::GetDevice(), &GraphicsLayoutCreateInfo, nullptr, &mPipelineLayout), "failed to create pipeline layout");
}

void FVulkanPipelineLayout::Release()
{
    if (mPipelineLayout)
        vkDestroyPipelineLayout(FVulkanDynamicRHI::GetDevice(), mPipelineLayout, nullptr);
    if (mDescriptorSetLayout)
        vkDestroyDescriptorSetLayout(FVulkanDynamicRHI::GetDevice(), mDescriptorSetLayout, nullptr);
}

void FVulkanPipeline::Build(const FVulkanPipelineLayout& InLayout, const FRHIGraphicsState& InState, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, const FVulkanRenderPass* RenderPass, const FVulkanPrimitive* InPrimitive)
{
    if (!InLayout.GetPipelineLayout())
        return;
    Release();
    FVulkanGraphicsPipelineBuilder Builder(InState);
    mPipeline = Builder.Build(FVulkanDynamicRHI::GetDevice(), InLayout.GetPipelineLayout(), InShaderStageInfo, RenderPass, InPrimitive);
    mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
}

FVulkanPipeline::FVulkanPipeline()
{
}

FVulkanPipeline::~FVulkanPipeline()
{
    Release();
}

void FVulkanPipeline::Release()
{
    if (mPipeline)
        vkDestroyPipeline(FVulkanDynamicRHI::GetDevice(), mPipeline, nullptr);
}

FVulkanPipelineCache::~FVulkanPipelineCache()
{
    Clear();
}

void FVulkanPipelineCache::Add(const FVulkanPipelineKey& InKey, const Ref<FVulkanPipeline>& InPipeline)
{
    mPipelineMap[InKey] = InPipeline;
}

Ref<FVulkanPipeline> FVulkanPipelineCache::Find(const FVulkanPipelineKey& InKey)
{
    if (auto Iter = mPipelineMap.find(InKey); Iter != mPipelineMap.end())
    {
        return Iter->second;
    }
    return nullptr;
}

void FVulkanPipelineCache::Clear()
{
    mPipelineMap.clear();
}

}