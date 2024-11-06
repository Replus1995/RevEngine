#include "VulkanEnum.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

std::pair<VkFilter, VkSamplerMipmapMode> FVulkanEnum::Translate(ESamplerFilterMode InMode)
{
    std::pair<VkFilter, VkSamplerMipmapMode> Result{};
    switch (InMode)
    {
    case SF_Nearest:
        Result.first = VK_FILTER_NEAREST;
        break;
    case SF_Bilinear:
    case SF_Trilinear:
    case SF_AnisotropicNearest:
    case SF_AnisotropicLinear:
        Result.first = VK_FILTER_LINEAR;
        break;
    default:
        REV_CORE_ASSERT(false, "Invalid filter mode");
        break;
    }

    switch (InMode)
    {
    case SF_Nearest:
        Result.second = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case SF_Bilinear:
        Result.second = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case SF_Trilinear:
        Result.second = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;
    case SF_AnisotropicNearest:
        Result.second = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        break;
    case SF_AnisotropicLinear:
        Result.second = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        break;
    default:
        REV_CORE_ASSERT(false, "Invalid mip mode");
        break;
    }

    return Result;
}

VkSamplerAddressMode FVulkanEnum::Translate(ESamplerWarpMode InMode)
{
    switch (InMode)
    {
    case Rev::SW_Repeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case Rev::SW_Clamp:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case Rev::SW_Mirror:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case Rev::SW_Border:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    default:
        break;
    }

    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}

VkPrimitiveTopology FVulkanEnum::Translate(EPrimitiveTopology InTopology)
{
    switch (InTopology)
    {
    default:
    case Rev::PT_Unknown:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case Rev::PT_Points:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case Rev::PT_Lines:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case Rev::PT_LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case Rev::PT_Triangles:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case Rev::PT_TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case Rev::PT_TriangleFan:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    }
}

VkStencilOp FVulkanEnum::Translate(EStencilOperation InOp)
{
    return VkStencilOp(InOp);
}

VkCompareOp FVulkanEnum::Translate(ECompareFunction InFunc)
{
    return VkCompareOp(InFunc);
}

VkBlendFactor FVulkanEnum::Translate(EBlendFactor InFactor)
{
    return VkBlendFactor(InFactor);
}

VkBlendOp FVulkanEnum::Translate(EBlendOperation InOp)
{
    return VkBlendOp(InOp);
}

VkColorComponentFlags FVulkanEnum::Translate(EColorWriteMask InMask)
{
    return VkColorComponentFlags(InMask);
}

VkPolygonMode FVulkanEnum::Translate(EFillMode InMode)
{
    return VkPolygonMode(InMode);
}

VkCullModeFlags FVulkanEnum::Translate(ECullMode InMode)
{
    switch (InMode)
    {
    default:
    case Rev::CM_None:
        return VK_CULL_MODE_NONE;
    case Rev::CM_Back:
        return VK_CULL_MODE_BACK_BIT;
    case Rev::CM_Front:
        return VK_CULL_MODE_FRONT_BIT;
    case Rev::CM_BackAndFront:
        return VK_CULL_MODE_FRONT_AND_BACK;
    }
}

VkAttachmentLoadOp FVulkanEnum::Translate(EAttachmentLoadOp InOp)
{
    return VkAttachmentLoadOp(InOp);
}

VkAttachmentStoreOp FVulkanEnum::Translate(EAttachmentStoreOp InOp)
{
    return VkAttachmentStoreOp(InOp);
}

VkPipelineBindPoint FVulkanEnum::Translate(EPipelineBindPoint InBindPoint)
{
    switch (InBindPoint)
    {
    default:
    case Rev::PBP_Graphics:
        return VK_PIPELINE_BIND_POINT_GRAPHICS;
    case Rev::PBP_Compute:
        return VK_PIPELINE_BIND_POINT_COMPUTE;
    case Rev::PBP_RayTracing:
        return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
    }
}

VkFormat FVulkanEnum::Translate(EVertexElmentType InType)
{
    switch (InType)
    {
    default:
    case EVertexElmentType::Unknown:
        break;
    case EVertexElmentType::Float1:
        return VK_FORMAT_R32_SFLOAT;
    case EVertexElmentType::Float2:
        return VK_FORMAT_R32G32_SFLOAT;
    case EVertexElmentType::Float3:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case EVertexElmentType::Float4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case EVertexElmentType::UByte4:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case EVertexElmentType::Color:
        return VK_FORMAT_R8G8B8A8_UINT;
    case EVertexElmentType::Matrix3:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case EVertexElmentType::Matrix4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case EVertexElmentType::UInt:
        return VK_FORMAT_R32_UINT;
    case EVertexElmentType::Bool:
        return VK_FORMAT_R8_SINT;
    }

    return VK_FORMAT_UNDEFINED;
}

}
