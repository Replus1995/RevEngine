#include "VkSampler.h"
#include "VkCore.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVkSampler::FVkSampler(const FSamplerDesc& InDesc)
	: FRHISampler(InDesc)
{
	CreateResource();
}

FVkSampler::~FVkSampler()
{
	vkDestroySampler(FVkCore::GetDevice(), mSampler, nullptr);
}

VkFilter FVkSampler::TranslateFilterMode(ESamplerFilterMode InMode)
{
	switch (InMode)
	{
	case SF_Nearest:
		return VK_FILTER_NEAREST;
	case SF_Bilinear:
	case SF_Trilinear:
	case SF_AnisotropicNearest:
	case SF_AnisotropicLinear:
		return VK_FILTER_LINEAR;
	}
	REV_CORE_ASSERT(false, "Invalid filter mode");
	return VkFilter(0);
}

VkSamplerMipmapMode FVkSampler::TranslateMipmapMode(ESamplerFilterMode InMode)
{
	switch (InMode)
	{
	case SF_Nearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case SF_Bilinear:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case SF_Trilinear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	case SF_AnisotropicNearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case SF_AnisotropicLinear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}
	REV_CORE_ASSERT(false, "Invalid mip mode");
	return VkSamplerMipmapMode(0);
}

VkSamplerAddressMode FVkSampler::TranslateWarpMode(ESamplerWarpMode InMode)
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

bool FVkSampler::UseAnisotropicFilter(const FSamplerDesc& InDesc)
{
	return InDesc.Filter >= SF_AnisotropicNearest;
}

bool FVkSampler::UseBorderWarp(const FSamplerDesc& InDesc)
{
	return InDesc.WarpU == SW_Border || InDesc.WarpV == SW_Border || InDesc.WarpW == SW_Border;
}

void FVkSampler::CreateResource()
{
	VkSamplerCreateInfo CreateInfo{};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	CreateInfo.magFilter = TranslateFilterMode(mDesc.Filter);
	CreateInfo.minFilter = TranslateFilterMode(mDesc.Filter);
	CreateInfo.mipmapMode = TranslateMipmapMode(mDesc.Filter);
	CreateInfo.addressModeU = TranslateWarpMode(mDesc.WarpU);
	CreateInfo.addressModeV = TranslateWarpMode(mDesc.WarpV);
	CreateInfo.addressModeW = TranslateWarpMode(mDesc.WarpW);
	CreateInfo.mipLodBias = 0.0f;
	CreateInfo.anisotropyEnable = UseAnisotropicFilter(mDesc);
	CreateInfo.maxAnisotropy = float(mDesc.Anisotropic);
	//CreateInfo.compareEnable = false;
	//CreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	//CreateInfo.minLod
	//CreateInfo.maxLod
	//CreateInfo.borderColor
	//CreateInfo.unnormalizedCoordinates

	REV_VK_CHECK(vkCreateSampler(FVkCore::GetDevice(), &CreateInfo, nullptr, &mSampler));
}

Ref<FVkSampler> CreateVkSampler(const FSamplerDesc& InSamplerDesc)
{
	return CreateRef<FVkSampler>(InSamplerDesc);
}

}


