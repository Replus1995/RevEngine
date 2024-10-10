#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{
class FVulkanRenderTarget;
class FVulkanPrimitive;
class FVulkanPipeline
{
public:
    FVulkanPipeline();
    ~FVulkanPipeline();

    void BuildLayout(const std::vector<VkDescriptorSetLayoutBinding>& InBindingInfo);
    void Build(const FRHIGraphicsPipelineState& InState, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, const FVulkanRenderTarget* InRenderTarget, const FVulkanPrimitive* InPrimitive);
    
    void ReleaseLayout();
    void Release();

    VkDescriptorSetLayout GetDescriptorSetLayout() const { return mDescriptorSetLayout; }
    VkPipelineLayout GetPipelineLayout() const { return mPipelineLayout; }
    VkPipeline GetPipeline() const { return mPipeline; }
    VkPipelineBindPoint GetPipelineBindPoint() const { return mPipelineBindPoint; }

private:
    VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;
    VkPipelineBindPoint mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
};

class FVulkanGraphicsPipelineBuilder
{
public:
    FVulkanGraphicsPipelineBuilder(const FRHIGraphicsPipelineState& InState);
    ~FVulkanGraphicsPipelineBuilder();

    VkPipeline Build(VkDevice InDevice, VkPipelineLayout InLayout, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, const FVulkanRenderTarget* InRenderTarget, const FVulkanPrimitive* InPrimitive);

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