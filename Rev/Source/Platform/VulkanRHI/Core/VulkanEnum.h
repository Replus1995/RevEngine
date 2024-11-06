#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHIRenderPass.h"
#include <vulkan/vulkan.h>


namespace Rev
{

class FVulkanEnum
{
public:
    //Sampler
    static std::pair<VkFilter, VkSamplerMipmapMode> Translate(ESamplerFilterMode InMode);
    static VkSamplerAddressMode Translate(ESamplerWarpMode InMode);

    //Pipeline
    static VkPrimitiveTopology Translate(EPrimitiveTopology InTopology);
    static VkStencilOp Translate(EStencilOperation InOp);
    static VkCompareOp Translate(ECompareFunction InFunc);
    static VkBlendFactor Translate(EBlendFactor InFactor);
    static VkBlendOp Translate(EBlendOperation InOp);
    static VkColorComponentFlags Translate(EColorWriteMask InMask);
    static VkPolygonMode Translate(EFillMode InMode);
    static VkCullModeFlags Translate(ECullMode InMode);
    static VkAttachmentLoadOp Translate(EAttachmentLoadOp InOp);
    static VkAttachmentStoreOp Translate(EAttachmentStoreOp InOp);
    static VkPipelineBindPoint Translate(EPipelineBindPoint InBindPoint);

    //Buffer
    static VkFormat Translate(EVertexElmentType InType);
};

}