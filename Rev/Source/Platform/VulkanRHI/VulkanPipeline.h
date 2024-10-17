#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <map>

namespace Rev
{
class FVulkanRenderPass;
class FVulkanPrimitive;

class FVulkanGraphicsPipelineBuilder
{
public:
    FVulkanGraphicsPipelineBuilder(const FRHIGraphicsState& InState);
    ~FVulkanGraphicsPipelineBuilder();

    VkPipeline Build(VkDevice InDevice, VkPipelineLayout InLayout, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, const FVulkanRenderPass* RenderPass, const FVulkanPrimitive* InPrimitive);

private:
    VkPipelineInputAssemblyStateCreateInfo MakeInputAssemblyStateInfo(VkPrimitiveTopology InTopology) const;
    VkPipelineTessellationStateCreateInfo MakeTessellationStateInfo() const;
    VkPipelineViewportStateCreateInfo MakeViewportStateInfo() const;
    VkPipelineRasterizationStateCreateInfo MakeRasterizationStateInfo() const;
    VkPipelineMultisampleStateCreateInfo MakeMultisampleStateInfo() const;
    VkPipelineDepthStencilStateCreateInfo MakeDepthStencilStateInfo() const;
    VkPipelineColorBlendStateCreateInfo MakeColorBlendStateInfo(const VkPipelineColorBlendAttachmentState* InColorBlendStates, uint32 InNumColorBlendStates) const;
    VkPipelineDynamicStateCreateInfo MakeDynamicStateInfo(const VkDynamicState* InDynaimcStates, uint32 InNumDynaimcStates) const;
    VkPipelineRenderingCreateInfo MakeRenderingInfo(const VkFormat* InColorFomats, uint32 InNumColorFormats, VkFormat InDepthFormat) const;

private:
    FRHIGraphicsState mState;
    
};

class FVulkanPipelineLayout
{
public:
    FVulkanPipelineLayout();
    ~FVulkanPipelineLayout();

    void Build(const std::vector<VkDescriptorSetLayoutBinding>& InBindingInfo);
    void Release();

    VkDescriptorSetLayout GetDescriptorSetLayout() const { return mDescriptorSetLayout; }
    VkPipelineLayout GetPipelineLayout() const { return mPipelineLayout; }

private:
    VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
};

class FVulkanPipeline
{
public:
    FVulkanPipeline();
    ~FVulkanPipeline();

    void Build(const FVulkanPipelineLayout& InLayout, const FRHIGraphicsState& InState, const std::vector<VkPipelineShaderStageCreateInfo>& InShaderStageInfo, const FVulkanRenderPass* RenderPass, const FVulkanPrimitive* InPrimitive);
    void Release();

    VkPipeline GetPipeline() const { return mPipeline; }
    VkPipelineBindPoint GetPipelineBindPoint() const { return mPipelineBindPoint; }

private:
    VkPipeline mPipeline = VK_NULL_HANDLE;
    VkPipelineBindPoint mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
};

struct FVulkanPipelineKey
{
    VkRenderPass RenderPass = VK_NULL_HANDLE;
    uint64 InputDescHash = 0;

    bool operator<(const FVulkanPipelineKey& Other) const {
        return RenderPass < Other.RenderPass && InputDescHash < Other.InputDescHash;
    }

    bool operator==(const FVulkanPipelineKey& Other) const
    {
        return RenderPass == Other.RenderPass && InputDescHash == Other.InputDescHash;
    }

    bool operator!=(const FVulkanPipelineKey& Other) const
    {
        return RenderPass != Other.RenderPass || InputDescHash != Other.InputDescHash;
    }
};

class FVulkanPipelineCache
{
public:
    FVulkanPipelineCache() = default;
    ~FVulkanPipelineCache();

    void Add(const FVulkanPipelineKey& InKey, const Ref<FVulkanPipeline>& InPipeline);
    Ref<FVulkanPipeline> Find(const FVulkanPipelineKey& InKey);
    void Clear();


private:
    std::map<FVulkanPipelineKey, Ref<FVulkanPipeline>> mPipelineMap;
};

}