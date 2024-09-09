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
    FVulkanGraphicsPipelineBuilder(const FRHIGraphicsPipelineState& InState);
    ~FVulkanGraphicsPipelineBuilder();

    VkPipeline Build(VkDevice InDevice, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo) const;

private:
    VkPipelineInputAssemblyStateCreateInfo MakeInputAssemblyStateInfo() const;
    VkPipelineTessellationStateCreateInfo MakeTessellationStateInfo() const;
    VkPipelineViewportStateCreateInfo MakeViewportStateInfo() const;
    VkPipelineRasterizationStateCreateInfo MakeRasterizationStateInfo() const;
    VkPipelineMultisampleStateCreateInfo MakeMultisampleStateInfo() const;
    VkPipelineDepthStencilStateCreateInfo MakeDepthStencilStateInfo() const;
    VkPipelineColorBlendStateCreateInfo MakeColorBlendStateInfo() const;
    VkPipelineDynamicStateCreateInfo MakeDynamicStateInfo() const;
    VkPipelineRenderingCreateInfo MakeRenderingInfo() const;

private:
    FRHIGraphicsPipelineState mState;
    std::vector<VkPipelineColorBlendAttachmentState> mAttachemntStateVec;
    VkDynamicState mDynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    //VkDynamicState mDynamicStates[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY };
    
};

}