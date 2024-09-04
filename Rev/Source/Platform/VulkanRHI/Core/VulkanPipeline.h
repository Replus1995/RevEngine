#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{

struct FVulkanPipelineBuilder
{
public:
    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;

    VkPipelineInputAssemblyStateCreateInfo InputAssembly;
    VkPipelineRasterizationStateCreateInfo Rasterizer;
    VkPipelineMultisampleStateCreateInfo Multisampling;
    VkPipelineLayout PipelineLayout;
    VkPipelineColorBlendAttachmentState ColorBlendAttachment;
    VkFormat ColorAttachmentformat = VK_FORMAT_UNDEFINED;
    VkPipelineDepthStencilStateCreateInfo DepthStencil;
    VkPipelineRenderingCreateInfo RenderInfo;

public:
    FVulkanPipelineBuilder();
    VkPipeline Build(VkDevice InDevice);
    void Clear();
}


}