#include "VulkanPipeline.h"
#include "VulkanDefines.h"

namespace Rev
{

VkPipelineInputAssemblyStateCreateInfo FVulkanGraphicsPipeline::MakeAssemblyStateInfo(const FRHIGraphicsPipelineState& InState)
{
    VkPipelineInputAssemblyStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    StateInfo.topology = TranslatePrimitiveTopology(InState.PrimitiveTopology);
    StateInfo.primitiveRestartEnable = VK_FALSE;


    return StateInfo;
}

VkPipelineViewportStateCreateInfo FVulkanGraphicsPipeline::MakeViewportStateInfo(const FRHIGraphicsPipelineState& InState)
{
    // make viewport state from our stored viewport and scissor.
    // at the moment we wont support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo StateInfo = {};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    StateInfo.pNext = nullptr;
    StateInfo.viewportCount = 1;
    StateInfo.scissorCount = 1;
    
    return StateInfo;
}

VkPipelineColorBlendStateCreateInfo FVulkanGraphicsPipeline::MakeColorBlendStateInfo(const FRHIGraphicsPipelineState& InState)
{
    std::vector<VkPipelineColorBlendAttachmentState> AttachemntStateVec;
    for (const auto& SrcAttachemntState : InState.ColorBlendAttachmentStates)
    {
        VkPipelineColorBlendAttachmentState AttachemntState{};
        AttachemntState.blendEnable = SrcAttachemntState.BlendEnable;
        AttachemntState.srcColorBlendFactor = TranslateBlendFactor(SrcAttachemntState.SrcColorBlendFactor);
        AttachemntState.dstColorBlendFactor = TranslateBlendFactor(SrcAttachemntState.DstColorBlendFactor);
        AttachemntState.colorBlendOp = TranslateBlendOp(SrcAttachemntState.ColorBlendOp);
        AttachemntState.srcAlphaBlendFactor = TranslateBlendFactor(SrcAttachemntState.SrcAlphaBlendFactor);
        AttachemntState.dstAlphaBlendFactor = TranslateBlendFactor(SrcAttachemntState.DstAlphaBlendFactor);
        AttachemntState.alphaBlendOp = TranslateBlendOp(SrcAttachemntState.AlphaBlendOp);
        AttachemntState.colorWriteMask = TranslateColorComponentMask(SrcAttachemntState.ColorWriteMask);
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

VkPrimitiveTopology FVulkanGraphicsPipeline::TranslatePrimitiveTopology(EPrimitiveTopology InTopology)
{
    switch (InTopology)
    {
    default:
    case Rev::PT_Unknown:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case Rev::PT_Points:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case Rev::PT_Lines:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case Rev::PT_LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case Rev::PT_Triangles:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case Rev::PT_TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case Rev::PT_TriangleFan:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    }
}

VkBlendFactor FVulkanGraphicsPipeline::TranslateBlendFactor(EBlendFactor InFactor)
{
    return VkBlendFactor(InFactor);
}

VkBlendOp FVulkanGraphicsPipeline::TranslateBlendOp(EBlendOp InOp)
{
    return VkBlendOp(InOp);
}

VkColorComponentFlags FVulkanGraphicsPipeline::TranslateColorComponentMask(EColorComponentMask InMask)
{
    return VkColorComponentFlags(InMask);
}

}