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
	static void FillCreateInfo(const FRHISamplerStateDesc& InDesc, VkSamplerCreateInfo& OutCreateInfo);

	VkSampler Sampler = VK_NULL_HANDLE;
};


class FVulkanRasterizerState : public FRHIRasterizerState
{
public:
	FVulkanRasterizerState(const FRHIRasterizerStateDesc& InDesc);
	virtual bool GetDesc(FRHIRasterizerStateDesc& OutDesc) const override final
	{
		OutDesc = Desc;
		return true;
	}

	VkPipelineRasterizationStateCreateInfo	RasterizerState;
	FRHIRasterizerStateDesc Desc;
};

class FVulkanDepthStencilState : public FRHIDepthStencilState
{
public:
	FVulkanDepthStencilState(const FRHIDepthStencilStateDesc& InDesc);
	virtual bool GetDesc(FRHIDepthStencilStateDesc& OutDesc) const override final
	{
		OutDesc = Desc;
		return true;
	}

	VkPipelineDepthStencilStateCreateInfo DepthStencilState;
	FRHIDepthStencilStateDesc Desc;
};

class FVulkanColorBlendState : public FRHIColorBlendState
{
public:
	FVulkanColorBlendState(const FRHIColorBlendStateDesc& InDesc);

	virtual bool GetDesc(FRHIColorBlendStateDesc& OutDesc) const override final
	{
		OutDesc = Desc;
		return true;
	}

	// array the pipeline state can point right to
	VkPipelineColorBlendAttachmentState ColorBlendStates[RTA_MaxColorAttachments];
	FRHIColorBlendStateDesc Desc;
};

}