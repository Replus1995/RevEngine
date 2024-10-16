#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHIPrimitive.h"
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RHI/RHIPipeline.h"
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
    static VkCompareOp Translate(ECompareOp InOp);
    static VkBlendFactor Translate(EBlendFactor InFactor);
    static VkBlendOp Translate(EBlendOp InOp);
    static VkColorComponentFlags Translate(EColorComponentMask InMask);
    static VkPolygonMode Translate(EPolygonMode InMode);
    static VkCullModeFlags Translate(ECullMode InMode);
    static VkAttachmentLoadOp Translate(EAttachmentLoadOp InOp);
    static VkAttachmentStoreOp Translate(EAttachmentStoreOp InOp);
    static VkPipelineBindPoint Translate(EPipelineBindPoint InBindPoint);

    //Buffer
    static VkFormat Translate(EVertexElementType InType);
};

}