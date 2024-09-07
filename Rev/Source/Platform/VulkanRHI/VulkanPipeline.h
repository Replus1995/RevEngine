#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{

struct FVulkanGraphicsPipelineBuilder
{
public:


private:
    VkPipelineInputAssemblyStateCreateInfo MakeInputAssemblyStateInfo(const FRHIGraphicsPipelineState& InState);
    //VkPipelineTessellationStateCreateInfo
    VkPipelineViewportStateCreateInfo MakeViewportStateInfo(const FRHIGraphicsPipelineState& InState);
    VkPipelineRasterizationStateCreateInfo MakeRasterizationStateInfo(const FRHIGraphicsPipelineState& InState);
    VkPipelineMultisampleStateCreateInfo
    VkPipelineDepthStencilStateCreateInfo MakeDepthStencilStateInfo(const FRHIGraphicsPipelineState& InState);
    VkPipelineColorBlendStateCreateInfo MakeColorBlendStateInfo(const FRHIGraphicsPipelineState& InState);
    VkPipelineDynamicStateCreateInfo MakeDynamicStateInfo();

private:
    VkDynamicState mDynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    //VkDynamicState mDynamicStates[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY };
    
};

}