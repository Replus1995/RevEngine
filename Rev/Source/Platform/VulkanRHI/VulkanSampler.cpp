#include "VulkanSampler.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanSampler::FVulkanSampler(const FSamplerDesc& InDesc)
	: FRHISampler(InDesc)
{
	CreateResource();
}

FVulkanSampler::~FVulkanSampler()
{
	vkDestroySampler(FVulkanDynamicRHI::GetDevice(), mSampler, nullptr);
}

bool FVulkanSampler::UseAnisotropicFilter(const FSamplerDesc& InDesc)
{
	return InDesc.Filter >= SF_AnisotropicNearest;
}

bool FVulkanSampler::UseBorderWarp(const FSamplerDesc& InDesc)
{
	return InDesc.WarpU == SW_Border || InDesc.WarpV == SW_Border || InDesc.WarpW == SW_Border;
}

void FVulkanSampler::CreateResource()
{
	auto FilterMode = FVulkanEnum::Translate(mDesc.Filter);

	VkSamplerCreateInfo CreateInfo{};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	CreateInfo.magFilter = FilterMode.first;
	CreateInfo.minFilter = FilterMode.first;
	CreateInfo.mipmapMode = FilterMode.second;
	CreateInfo.addressModeU = FVulkanEnum::Translate(mDesc.WarpU);
	CreateInfo.addressModeV = FVulkanEnum::Translate(mDesc.WarpV);
	CreateInfo.addressModeW = FVulkanEnum::Translate(mDesc.WarpW);
	CreateInfo.mipLodBias = 0.0f;
	CreateInfo.anisotropyEnable = UseAnisotropicFilter(mDesc);
	CreateInfo.maxAnisotropy = float(mDesc.Anisotropic);
	//CreateInfo.compareEnable = false;
	//CreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	//CreateInfo.minLod
	//CreateInfo.maxLod
	//CreateInfo.borderColor
	//CreateInfo.unnormalizedCoordinates

	REV_VK_CHECK(vkCreateSampler(FVulkanDynamicRHI::GetDevice(), &CreateInfo, nullptr, &mSampler));
}

Ref<FVulkanSampler> CreateVkSampler(const FSamplerDesc& InSamplerDesc)
{
	return CreateRef<FVulkanSampler>(InSamplerDesc);
}

}


