#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Hash.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Core/VulkanDefines.h"
#include <vulkan/vulkan.h>
#include <map>
#include <unordered_map>

namespace Rev
{
class FVulkanRenderPass;
class FVulkanShaderProgram;

class FVulkanPipelineLayout
{
public:
    FVulkanPipelineLayout();
    ~FVulkanPipelineLayout();

    void Build(const VkDescriptorSetLayoutBinding* InBindingInfo, uint32 InBindingsCount);
    void Release();

    VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
};

class FVulkanPipeline
{
public:
    FVulkanPipeline();
    ~FVulkanPipeline();

    void BuildGraphics(FVulkanPipelineLayout* InLayout, 
        const FRHIGraphicsPipelineStateDesc& InStateDesc,
        const FVulkanRenderPass* RenderPass,
        const FVulkanShaderProgram* InProgram);

    void Release();

    VkPipeline Pipeline = VK_NULL_HANDLE;
    FVulkanPipelineLayout* PipelineLayout = nullptr;
};

struct FVulkanPipelineLayoutDesc
{
    VkDescriptorSetLayoutBinding Bindings[REV_VK_MAX_DESCRIPTORSETS];
    uint32 NumBindings;

    FVulkanPipelineLayoutDesc(const FVulkanShaderProgram* InProgram);

    friend bool operator==(const FVulkanPipelineLayoutDesc& A, const FVulkanPipelineLayoutDesc& B);
};

struct FVulkanGraphicsPipelineDesc
{
    FRHIGraphicsPipelineStateDesc PipelineState;
    VkRenderPass RenderPass;
    VkShaderModule ShaderModules[SS_NumGraphics];

    FVulkanGraphicsPipelineDesc(const FRHIGraphicsPipelineStateDesc& InPipelineState, const FVulkanRenderPass* InRenderPass, const FVulkanShaderProgram* InProgram);

    friend bool operator==(const FVulkanGraphicsPipelineDesc& A, const FVulkanGraphicsPipelineDesc& B);
};

}

STD_MAP_HASH(Rev::FVulkanPipelineLayoutDesc)
STD_MAP_HASH(Rev::FVulkanGraphicsPipelineDesc)
bool operator==(const VkDescriptorSetLayoutBinding& A, const VkDescriptorSetLayoutBinding& B);

namespace Rev
{

class FVulkanGraphicsPipelineCache
{
public:
    FVulkanGraphicsPipelineCache() = default;
    ~FVulkanGraphicsPipelineCache();

    FVulkanPipeline* GetOrCreatePipeline(
        const FRHIGraphicsPipelineStateDesc& InStateDesc, 
        const FVulkanRenderPass* InRenderPass,
        const FVulkanShaderProgram* InProgram);

    void ClearAll();

private:
    std::unordered_map<FVulkanPipelineLayoutDesc, Scope<FVulkanPipelineLayout>> mLayoutCache;
    std::unordered_map<FVulkanGraphicsPipelineDesc, Scope<FVulkanPipeline>> mPipelineCache;
};

}