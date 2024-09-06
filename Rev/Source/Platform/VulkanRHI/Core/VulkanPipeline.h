#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{

struct FVulkanGraphicsPipeline
{
public:
    static VkPipelineInputAssemblyStateCreateInfo MakeAssemblyStateInfo(const FRHIGraphicsPipelineState& InState);
    //VkPipelineTessellationStateCreateInfo
    static VkPipelineViewportStateCreateInfo MakeViewportStateInfo(const FRHIGraphicsPipelineState& InState);
    static VkPipelineRasterizationStateCreateInfo MakeRasterizationStateInfo(const FRHIGraphicsPipelineState& InState);
    //VkPipelineMultisampleStateCreateInfo
    static VkPipelineDepthStencilStateCreateInfo MakeDepthStencilStateInfo(const FRHIGraphicsPipelineState& InState);
    static VkPipelineColorBlendStateCreateInfo MakeColorBlendStateInfo(const FRHIGraphicsPipelineState& InState);

private:
    static VkPrimitiveTopology TranslatePrimitiveTopology(EPrimitiveTopology InTopology);
    static VkBlendFactor TranslateBlendFactor(EBlendFactor InFactor);
    static VkBlendOp TranslateBlendOp(EBlendOp InOp);
    static VkColorComponentFlags TranslateColorComponentMask(EColorComponentMask InMask);
};

}