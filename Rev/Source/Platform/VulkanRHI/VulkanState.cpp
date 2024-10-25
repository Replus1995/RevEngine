#include "VulkanState.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Hash.h"

namespace Rev
{

//Dynamic RHI

Ref<FRHISamplerState> FVulkanDynamicRHI::RHICreateSamplerState(const FRHISamplerStateDesc& InDesc)
{
	VkSamplerCreateInfo SamplerCreateInfo;
	FVulkanSamplerState::FillCreateInfo(InDesc, SamplerCreateInfo);

	uint32 hash = FCityHash::Gen(&SamplerCreateInfo, sizeof(SamplerCreateInfo));

	//Add lock for multithread create
	{
		if (auto iter = mSamplerMap.find(hash); iter != mSamplerMap.end())
		{
			return iter->second;
		}
		Ref<FVulkanSamplerState> NewSampler = CreateRef<FVulkanSamplerState>(SamplerCreateInfo);
		mSamplerMap.emplace(hash, NewSampler);
		return NewSampler;
	}
}

Ref<FRHIRasterizerState> FVulkanDynamicRHI::RHICreateRasterizerState(const FRHIRasterizerStateDesc& InDesc)
{
	return CreateRef<FVulkanRasterizerState>(InDesc);
}

Ref<FRHIDepthStencilState> FVulkanDynamicRHI::RHICreateDepthStencilStateState(const FRHIDepthStencilStateDesc& InDesc)
{
	return CreateRef<FVulkanDepthStencilState>(InDesc);
}

Ref<FRHIColorBlendState> FVulkanDynamicRHI::RHICreateColorBlendState(const FRHIColorBlendStateDesc& InDesc)
{
	return CreateRef<FVulkanColorBlendState>(InDesc);
}

Ref<FRHIVertexInputState> FVulkanDynamicRHI::RHICreateVertexInputState(const FRHIVertexInputStateDesc& InDesc)
{
	return CreateRef<FVulkanVertexInputState>(InDesc);
}
//Dynamic RHI



FVulkanSamplerState::FVulkanSamplerState(const VkSamplerCreateInfo& InCreateInfo)
{
	REV_VK_CHECK(vkCreateSampler(FVulkanDynamicRHI::GetDevice(), &InCreateInfo, nullptr, &Sampler));
}

FVulkanSamplerState::~FVulkanSamplerState()
{
	vkDestroySampler(FVulkanDynamicRHI::GetDevice(), Sampler, nullptr);
}

void FVulkanSamplerState::FillCreateInfo(const FRHISamplerStateDesc& InDesc, VkSamplerCreateInfo& OutCreateInfo)
{
	ZeroVulkanStruct(OutCreateInfo, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
	auto FilterMode = FVulkanEnum::Translate(InDesc.Filter);

	OutCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	OutCreateInfo.magFilter = FilterMode.first;
	OutCreateInfo.minFilter = FilterMode.first;
	OutCreateInfo.mipmapMode = FilterMode.second;
	OutCreateInfo.addressModeU = FVulkanEnum::Translate(InDesc.WarpU);
	OutCreateInfo.addressModeV = FVulkanEnum::Translate(InDesc.WarpV);
	OutCreateInfo.addressModeW = FVulkanEnum::Translate(InDesc.WarpW);

	OutCreateInfo.mipLodBias = InDesc.MipBias;
	
	OutCreateInfo.maxAnisotropy = 1.0f;
	if (InDesc.Filter == SF_AnisotropicLinear || InDesc.Filter == SF_AnisotropicNearest)
	{
		//OutCreateInfo.maxAnisotropy = FMath::Clamp((float)ComputeAnisotropyRT(Initializer.MaxAnisotropy), 1.0f, InDevice.GetLimits().maxSamplerAnisotropy);
		OutCreateInfo.maxAnisotropy = (float)InDesc.MaxAnisotropy;
	}
	OutCreateInfo.anisotropyEnable = OutCreateInfo.maxAnisotropy > 1.0f;

	OutCreateInfo.compareEnable = InDesc.CompareFunc != SCF_Never ? VK_TRUE : VK_FALSE;
	OutCreateInfo.compareOp = VkCompareOp(InDesc.CompareFunc);
	OutCreateInfo.minLod = InDesc.MinMipLevel;
	OutCreateInfo.maxLod = InDesc.MaxMipLevel;
	OutCreateInfo.borderColor = InDesc.BorderColor == 0 ? VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK : VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
}

FVulkanRasterizerState::FVulkanRasterizerState(const FRHIRasterizerStateDesc& InDesc)
	: Desc(InDesc)
{
	ZeroVulkanStruct(RasterizerState, VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
	//RasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
	RasterizerState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	RasterizerState.lineWidth = 1.0f;


	RasterizerState.polygonMode = FVulkanEnum::Translate(Desc.FillMode);
	RasterizerState.cullMode = FVulkanEnum::Translate(Desc.CullMode);

	RasterizerState.depthClampEnable = Desc.DepthClipMode == DCM_Clamp ? VK_TRUE : VK_FALSE;
	RasterizerState.depthBiasEnable = Desc.DepthBias != 0.0f ? VK_TRUE : VK_FALSE;
	//RasterizerState.rasterizerDiscardEnable = VK_FALSE;

	RasterizerState.depthBiasSlopeFactor = Desc.DepthBiasSlopeFactor;
	RasterizerState.depthBiasConstantFactor = Desc.DepthBias;
}

FVulkanDepthStencilState::FVulkanDepthStencilState(const FRHIDepthStencilStateDesc& InDesc)
	: Desc(InDesc)
{
	ZeroVulkanStruct(DepthStencilState, VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO);

	DepthStencilState.depthTestEnable = (Desc.DepthTestFunc != CF_Always || Desc.bEnableDepthWrite) ? VK_TRUE : VK_FALSE;
	DepthStencilState.depthCompareOp = FVulkanEnum::Translate(Desc.DepthTestFunc);
	DepthStencilState.depthWriteEnable = Desc.bEnableDepthWrite ? VK_TRUE : VK_FALSE;

	{
		//DepthStencilState.depthBoundsTestEnable = GfxPSOInit.bDepthBounds;
		/*DepthStencilState.depthBoundsTestEnable = false;
		DepthStencilState.minDepthBounds = 0.0f;
		DepthStencilState.maxDepthBounds = 1.0f;*/
	}

	DepthStencilState.stencilTestEnable = (Desc.bEnableFrontFaceStencil || Desc.bEnableBackFaceStencil) ? VK_TRUE : VK_FALSE;

	if (DepthStencilState.stencilTestEnable)
	{
		// Front
		DepthStencilState.back.failOp = FVulkanEnum::Translate(Desc.FrontFaceStencilFailOp);
		DepthStencilState.back.passOp = FVulkanEnum::Translate(Desc.FrontFaceStencilPassOp);
		DepthStencilState.back.depthFailOp = FVulkanEnum::Translate(Desc.FrontFaceStencilDepthFailOp);
		DepthStencilState.back.compareOp = FVulkanEnum::Translate(Desc.FrontFaceStencilFunc);
		DepthStencilState.back.compareMask = Desc.StencilReadMask;
		DepthStencilState.back.writeMask = Desc.StencilWriteMask;
		DepthStencilState.back.reference = 0;

		if (Desc.bEnableBackFaceStencil)
		{
			// Back
			DepthStencilState.front.failOp = FVulkanEnum::Translate(Desc.BackFaceStencilFailOp);
			DepthStencilState.front.passOp = FVulkanEnum::Translate(Desc.BackFaceStencilPassOp);
			DepthStencilState.front.depthFailOp = FVulkanEnum::Translate(Desc.BackFaceStencilDepthFailOp);
			DepthStencilState.front.compareOp = FVulkanEnum::Translate(Desc.BackFaceStencilFunc);
			DepthStencilState.front.compareMask = Desc.StencilReadMask;
			DepthStencilState.front.writeMask = Desc.StencilWriteMask;
			DepthStencilState.front.reference = 0;
		}
		else
		{
			DepthStencilState.front = DepthStencilState.back;
		}
	}
	
}

FVulkanColorBlendState::FVulkanColorBlendState(const FRHIColorBlendStateDesc& InDesc)
	: Desc(InDesc)
{
	for (uint32 Index = 0; Index < RTA_MaxColorAttachments; ++Index)
	{
		const FRHIColorBlendStateDesc::FAttachment& ColorTarget = Desc.Attachments[Index];
		VkPipelineColorBlendAttachmentState& ColorBlendState = Attachments[Index];
		memset((uint8*)&ColorBlendState, 0, sizeof(ColorBlendState));

		ColorBlendState.colorBlendOp = FVulkanEnum::Translate(ColorTarget.ColorOp);
		ColorBlendState.alphaBlendOp = FVulkanEnum::Translate(ColorTarget.AlphaOp);

		ColorBlendState.dstColorBlendFactor = FVulkanEnum::Translate(ColorTarget.DstColorFactor);
		ColorBlendState.dstAlphaBlendFactor = FVulkanEnum::Translate(ColorTarget.DstAlphaFactor);

		ColorBlendState.srcColorBlendFactor = FVulkanEnum::Translate(ColorTarget.SrcColorFactor);
		ColorBlendState.srcAlphaBlendFactor = FVulkanEnum::Translate(ColorTarget.SrcAlphaFactor);

		ColorBlendState.blendEnable = ColorTarget.bEnableBlend;
		ColorBlendState.colorWriteMask = FVulkanEnum::Translate(ColorTarget.ColorWriteMask);
	}
}

FVulkanVertexInputState::FVulkanVertexInputState(const FRHIVertexInputStateDesc& InDesc)
	: Desc(InDesc)
{
	REV_CORE_ASSERT(Desc.NumVertexElements < REV_MAX_VERTEX_ELEMENTS);

	std::map<uint32, uint32> BindingsMap;//<StreamIndex,Stride>
	for (uint8 i = 0; i < Desc.NumVertexElements; i++)
	{
		const FVertexElement& VertexElement = Desc.VertexElements[i];
		REV_CORE_ASSERT(VertexElement.StreamIndex < REV_MAX_VERTEX_ELEMENTS);
		REV_CORE_ASSERT(VertexElement.AttributeIndex < REV_MAX_VERTEX_ELEMENTS);

		BindingsMap[VertexElement.StreamIndex] = VertexElement.Stride;


		VkVertexInputAttributeDescription& AttributeDesc = Attributes[NumAttributes];
		AttributeDesc.binding = (uint32_t)VertexElement.StreamIndex;
		AttributeDesc.location = (uint32_t)VertexElement.AttributeIndex;
		AttributeDesc.format = FVulkanEnum::Translate(VertexElement.Type);
		AttributeDesc.offset = VertexElement.Offset;
		NumAttributes++;
	}

	for (auto[StreamIndex, Stride] : BindingsMap)
	{
		VkVertexInputBindingDescription& BindingDesc = Bindings[NumBindings];
		BindingDesc.binding = StreamIndex;
		BindingDesc.stride = Stride;
		BindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		NumBindings++;
	}

	VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputState.vertexBindingDescriptionCount = NumBindings;
	VertexInputState.pVertexBindingDescriptions = Bindings;
	VertexInputState.vertexAttributeDescriptionCount = NumAttributes;
	VertexInputState.pVertexAttributeDescriptions = Attributes;
}

void FVulkanGraphicsFrameState::PrepareForDraw(VkCommandBuffer InCmdBuffer)
{
	if (bVertexStreamsDirty)
	{
		VkBuffer VertexBuffers[REV_MAX_VERTEX_ELEMENTS];
		VkDeviceSize VertexOffsets[REV_MAX_VERTEX_ELEMENTS];
		uint8 NumStreams = 0;

		uint8 MaxStreamIndex = 0;
		for (uint32 i = 0; i < CurrentState.VertexInputStateDesc.NumVertexElements; i++)
		{
			const FVertexElement& Element = CurrentState.VertexInputStateDesc.VertexElements[i];
			MaxStreamIndex = Math::Max(MaxStreamIndex, Element.StreamIndex);
		}
		REV_CORE_ASSERT(MaxStreamIndex < REV_MAX_VERTEX_ELEMENTS);

		for (uint8 i = 0; i < MaxStreamIndex + 1; i++)
		{
			const FVulkanVertexStream& Stream = VertexStreams[i];
			if (Stream.Buffer == VK_NULL_HANDLE)
				continue;
			VertexBuffers[NumStreams] = Stream.Buffer;
			VertexOffsets[NumStreams] = Stream.Offset;
			++NumStreams;
		}

		vkCmdBindVertexBuffers2(InCmdBuffer, 0, NumStreams, VertexBuffers, VertexOffsets, NULL, NULL);

		bVertexStreamsDirty = false;
	}
}

}


