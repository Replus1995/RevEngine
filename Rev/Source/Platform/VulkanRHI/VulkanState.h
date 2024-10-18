#pragma once
#include "Rev/Render/RHI/RHIState.h"
#include <vulkan/vulkan.h>

namespace Rev
{
class FVulkanSamplerState : public FRHISamplerState
{
public:
	FVulkanSamplerState(const VkSamplerCreateInfo& InCreateInfo);
	virtual ~FVulkanSamplerState();
	static void FillCreateInfo(const FSamplerStateDesc& InDesc, VkSamplerCreateInfo& OutCreateInfo);

	VkSampler Sampler = VK_NULL_HANDLE;
};


class FVulkanRasterizerState : public FRHIRasterizerState
{
public:
	FVulkanRasterizerState(const FRasterizerStateDesc& InDesc);
	virtual bool GetDesc(FRasterizerStateDesc& OutDesc) const override final
	{
		OutDesc = Desc;
		return true;
	}

	VkPipelineRasterizationStateCreateInfo	RasterizerState;
	FRasterizerStateDesc Desc;
};

class FVulkanDepthStencilState : public FRHIDepthStencilState
{
public:
	FVulkanDepthStencilState(const FDepthStencilStateDesc& InDesc);
	virtual bool GetDesc(FDepthStencilStateDesc& OutDesc) const override final
	{
		OutDesc = Desc;
		return true;
	}

	VkPipelineDepthStencilStateCreateInfo DepthStencilState;
	FDepthStencilStateDesc Desc;
};

class FVulkanColorBlendState : public FRHIColorBlendState
{
public:
	FVulkanColorBlendState(const FColorBlendStateDesc& InDesc);

	virtual bool GetDesc(FColorBlendStateDesc& OutDesc) const override final
	{
		OutDesc = Desc;
		return true;
	}

	// array the pipeline state can point right to
	VkPipelineColorBlendAttachmentState ColorBlendStates[RTA_MaxColorAttachments];
	FColorBlendStateDesc Desc;
};

}