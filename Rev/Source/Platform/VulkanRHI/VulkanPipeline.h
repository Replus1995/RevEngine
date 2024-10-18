#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <map>

namespace Rev
{
class FVulkanRenderPass;
class FVulkanShaderProgram;
class FVulkanPrimitive;

class FVulkanGraphicsPipelineBuilder
{
public:
    static VkPipeline Build(VkDevice InDevice, VkPipelineLayout InLayout,
        const FRHIGraphicsPipelineStateDesc& InStateDesc,
        const FVulkanRenderPass* RenderPass,
        const FVulkanShaderProgram* InProgram,
        const FVulkanPrimitive* InPrimitive);

private:
    static VkPipelineInputAssemblyStateCreateInfo MakeInputAssemblyStateInfo(VkPrimitiveTopology InTopology);
    static VkPipelineTessellationStateCreateInfo MakeTessellationStateInfo();
    static VkPipelineViewportStateCreateInfo MakeViewportStateInfo();
    static VkPipelineMultisampleStateCreateInfo MakeMultisampleStateInfo();
    static VkPipelineDepthStencilStateCreateInfo MakeDepthStencilStateInfo();
    static VkPipelineColorBlendStateCreateInfo MakeColorBlendStateInfo(const VkPipelineColorBlendAttachmentState* InColorBlendStates, uint32 InNumColorBlendStates);
    static VkPipelineDynamicStateCreateInfo MakeDynamicStateInfo(const VkDynamicState* InDynaimcStates, uint32 InNumDynaimcStates);
    static VkPipelineRenderingCreateInfo MakeRenderingInfo(const VkFormat* InColorFomats, uint32 InNumColorFormats, VkFormat InDepthFormat);
    
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

    void Build(const FVulkanPipelineLayout& InLayout, 
        const FRHIGraphicsPipelineStateDesc& InStateDesc,
        const FVulkanRenderPass* RenderPass,
        const FVulkanShaderProgram* InProgram, 
        const FVulkanPrimitive* InPrimitive);

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

class FVulkanGraphicsPipelineCache
{
public:
    FVulkanGraphicsPipelineCache() = default;
    ~FVulkanGraphicsPipelineCache();

    VkPipeline GetOrCreatePipeline(
        const FRHIGraphicsPipelineStateDesc& InStateDesc, 
        const FVulkanRenderPass* InRenderPass,
        const FVulkanShaderProgram* InProgram,
        const FVulkanPrimitive* InPrimitive);

    void ClearAll();


private:
    std::map<uint32, FVulkanPipelineLayout> mLayoutCache;
    std::map<FVulkanPipelineKey, Ref<FVulkanPipeline>> mPipelineMap;
};

}