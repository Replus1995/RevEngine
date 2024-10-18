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

class FVulkanPipelineLayout
{
public:
    FVulkanPipelineLayout();
    ~FVulkanPipelineLayout();

    void Build(const std::vector<VkDescriptorSetLayoutBinding>& InBindingInfo);
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
        const FVulkanShaderProgram* InProgram, 
        const FVulkanPrimitive* InPrimitive);

    void Release();

    VkPipeline Pipeline = VK_NULL_HANDLE;
    FVulkanPipelineLayout* PipelineLayout = nullptr;
};

class FVulkanGraphicsPipelineCache
{
public:
    FVulkanGraphicsPipelineCache() = default;
    ~FVulkanGraphicsPipelineCache();

    FVulkanPipeline* GetOrCreatePipeline(
        const FRHIGraphicsPipelineStateDesc& InStateDesc, 
        const FVulkanRenderPass* InRenderPass,
        const FVulkanShaderProgram* InProgram,
        const FVulkanPrimitive* InPrimitive);

    void ClearAll();

    struct FCacheKey
    {
        uint32 StateHash = 0;
        VkRenderPass RenderPass = VK_NULL_HANDLE;
        uint32 ShaderHash[5];
        EPrimitiveTopology PrimitiveTopology = PT_Unknown;
        uint64 VertexHash = 0;
        uint32 SecondaryHash = 0;

        FCacheKey(const FRHIGraphicsPipelineStateDesc& InStateDesc,
            const FVulkanRenderPass* InRenderPass,
            const FVulkanShaderProgram* InProgram,
            const FVulkanPrimitive* InPrimitive);

        friend bool operator==(const FVulkanGraphicsPipelineCache::FCacheKey& L, const FVulkanGraphicsPipelineCache::FCacheKey& R);
        friend bool operator<(const FVulkanGraphicsPipelineCache::FCacheKey& L, const FVulkanGraphicsPipelineCache::FCacheKey& R);
    };

private:
    std::map<uint32, Scope<FVulkanPipelineLayout>> mLayoutCache;
    std::map<FCacheKey, Scope<FVulkanPipeline>> mPipelineCache;
};

}