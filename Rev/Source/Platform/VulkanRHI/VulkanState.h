#pragma once
#include "Rev/Render/RHI/RHIState.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include <vulkan/vulkan.h>
#include <map>

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
	VkPipelineColorBlendAttachmentState Attachments[RTA_MaxColorAttachments];
	FRHIColorBlendStateDesc Desc;
};

class FVulkanVertexInputState : public FRHIVertexInputState
{
public:
	FVulkanVertexInputState(const FRHIVertexInputStateDesc& InDesc);
	virtual bool GetDesc(FRHIVertexInputStateDesc& OutDesc) const override final
	{
		OutDesc = Desc;
		return true;
	}

	VkVertexInputBindingDescription Bindings[REV_MAX_VERTEX_ELEMENTS];
	uint32 NumBindings = 0;
	VkVertexInputAttributeDescription Attributes[REV_MAX_VERTEX_ELEMENTS];
	uint32 NumAttributes = 0;
	VkPipelineVertexInputStateCreateInfo VertexInputState = {};
	FRHIVertexInputStateDesc Desc;
};


struct FVulkanVertexStream
{
	VkBuffer Buffer = VK_NULL_HANDLE;
	uint32 Offset = 0;
};

class FVulkanRenderPass;
class FVulkanShaderProgram;
class FVulkanUniformBuffer;
class FVulkanTexture;
class FVulkanSamplerState;

struct FVulkanTextureSlot
{
	FVulkanTexture* Images[REV_MAX_UNIFORM_ARRAY_SIZE];
	FVulkanSamplerState* SamplerState = nullptr;
	uint32 NumImages = 0;

	FVulkanTextureSlot() = default;

	FVulkanTextureSlot(FVulkanTexture* InImage, FVulkanSamplerState* InSamplerState = nullptr)
	{
		Images[0] = InImage;
		SamplerState = InSamplerState;
		NumImages = 1;
 	}

	FVulkanTextureSlot(FVulkanTexture** InImages, uint32 InNumImages)
	{
		REV_CORE_ASSERT(InNumImages <= REV_MAX_UNIFORM_ARRAY_SIZE);

		for (uint32 i = 0; i < InNumImages; i++)
		{
			Images[i] = InImages[i];
		}
		NumImages = InNumImages;
	}
};

struct FVulkanGraphicsFrameState
{
	FVulkanRenderPass* CurrentPass = nullptr;
	FVulkanShaderProgram* CurrentProgram = nullptr;
	FRHIGraphicsPipelineStateDesc CurrentState = {};

	FVulkanVertexStream VertexStreams[REV_MAX_VERTEX_ELEMENTS];
	bool bVertexStreamsDirty = true;

	std::map<uint16, FVulkanUniformBuffer*> UniformBuffers;
	std::map<uint16, FVulkanTextureSlot> Textures;

	void Reset()
	{
		Textures.clear();
		UniformBuffers.clear();
		CurrentProgram = nullptr;
		CurrentPass = nullptr;
	}

	bool ReadyForDraw()
	{
		return CurrentPass != nullptr && CurrentProgram != nullptr;
	}

	void PrepareForDraw(VkCommandBuffer InCmdBuffer);

	FVulkanUniformBuffer* FindUniformBuffer(uint16 BindingIdx) const
	{
		if (auto iter = UniformBuffers.find(BindingIdx); iter != UniformBuffers.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	const FVulkanTextureSlot* FindTexture(uint16 BindingIdx) const
	{
		if (auto iter = Textures.find(BindingIdx); iter != Textures.end())
		{
			return &iter->second;
		}
		return nullptr;
	}
};

}