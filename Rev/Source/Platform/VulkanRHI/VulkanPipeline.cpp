#include "VulkanPipeline.h"
#include "VulkanDefines.h"
#include "VulkanEnum.h"

namespace Rev
{

VkPipelineInputAssemblyStateCreateInfo FVulkanGraphicsPipeline::MakeInputAssemblyStateInfo(const FRHIGraphicsPipelineState& InState)
{
    VkPipelineInputAssemblyStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    StateInfo.topology = FVulkanEnum::Translate(InState.PrimitiveTopology);
    StateInfo.primitiveRestartEnable = VK_FALSE;
    return StateInfo;
}

VkPipelineViewportStateCreateInfo FVulkanGraphicsPipeline::MakeViewportStateInfo(const FRHIGraphicsPipelineState& InState)
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

VkPipelineRasterizationStateCreateInfo FVulkanGraphicsPipeline::MakeRasterizationStateInfo(const FRHIGraphicsPipelineState& InState)
{
    VkPipelineRasterizationStateCreateInfo StateInfo{};
    StateInfo.depthClampEnable = InState.DepthClampEnable;
    StateInfo.rasterizerDiscardEnable = VK_FALSE;
    StateInfo.polygonMode = FVulkanEnum::Translate(InState.PolygonMode);
    StateInfo.cullMode = FVulkanEnum::Translate(InState.CullMode);
    StateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    StateInfo.depthBiasEnable = InState.DepthBiasEnable;
    StateInfo.depthBiasConstantFactor = InState.DepthBiasConstantFactor;
    StateInfo.depthBiasClamp = InState.DepthBiasClamp;
    StateInfo.depthBiasSlopeFactor = InState.DepthBiasSlopeFactor;
    StateInfo.lineWidth = InState.LineWidth;
    return StateInfo;
}

VkPipelineDepthStencilStateCreateInfo FVulkanGraphicsPipeline::MakeDepthStencilStateInfo(const FRHIGraphicsPipelineState& InState)
{
    VkPipelineDepthStencilStateCreateInfo StateInfo{};
    StateInfo.depthTestEnable = InState.DepthTestEnable;
    StateInfo.depthWriteEnable = InState.DepthWriteEnable;
    StateInfo.depthCompareOp = FVulkanEnum::Translate(InState.DepthCompareOp);
    //TODO: fill stencil state options
    return StateInfo;
}

VkPipelineColorBlendStateCreateInfo FVulkanGraphicsPipeline::MakeColorBlendStateInfo(const FRHIGraphicsPipelineState& InState)
{
    std::vector<VkPipelineColorBlendAttachmentState> AttachemntStateVec;
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
    }

    // setup dummy color blending. We arent using transparent objects yet
    // the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo StateInfo = {};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    StateInfo.pNext = nullptr;

    StateInfo.logicOpEnable = VK_FALSE;
    StateInfo.logicOp = VK_LOGIC_OP_COPY;
    StateInfo.attachmentCount = (uint32_t)AttachemntStateVec.size();
    StateInfo.pAttachments = AttachemntStateVec.data();

    for (size_t i = 0; i < 4; i++)
    {
        StateInfo.blendConstants[i] = InState.BlendConstants[i];
    }

    return StateInfo;
}



}