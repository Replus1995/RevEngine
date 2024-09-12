#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{

class FVulkanPipeline
{
public:
    FVulkanPipeline();
    ~FVulkanPipeline();

    void Build(const FRHIGraphicsPipelineState& InState, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, uint32 InColorAttachmentCount);
    void Release();

    VkPipelineLayout GetPipelineLayout() const { return mPipelineLayout; }
    VkPipeline GetPipeline() const { return mPipeline; }

private:
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;
};

class FVulkanGraphicsPipelineBuilder
{
public:
    FVulkanGraphicsPipelineBuilder(const FRHIGraphicsPipelineState& InState);
    ~FVulkanGraphicsPipelineBuilder();

    VkPipelineLayout BuildLayout(VkDevice InDevice);
    VkPipeline Build(VkDevice InDevice, VkPipelineLayout InLayout, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, uint32 InColorAttachmentCount = 1);

private:
    VkPipelineInputAssemblyStateCreateInfo MakeInputAssemblyStateInfo() const;
    VkPipelineTessellationStateCreateInfo MakeTessellationStateInfo() const;
    VkPipelineViewportStateCreateInfo MakeViewportStateInfo() const;
    VkPipelineRasterizationStateCreateInfo MakeRasterizationStateInfo() const;
    VkPipelineMultisampleStateCreateInfo MakeMultisampleStateInfo() const;
    VkPipelineDepthStencilStateCreateInfo MakeDepthStencilStateInfo() const;
    VkPipelineColorBlendStateCreateInfo MakeColorBlendStateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& InAttachmentStates) const;
    VkPipelineDynamicStateCreateInfo MakeDynamicStateInfo(const std::vector<VkDynamicState>& InDynaimcStates) const;
    VkPipelineRenderingCreateInfo MakeRenderingInfo() const;

private:
    FRHIGraphicsPipelineState mState;
    
};

}