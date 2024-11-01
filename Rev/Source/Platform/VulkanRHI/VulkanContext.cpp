#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanTexture2D.h"
#include "VulkanState.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanPipeline.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"

#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include "Rev/Render/RHI/RHIUtils.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

//#include <VkBootstrap.h>

namespace Rev
{

FVulkanContext::FVulkanContext()
{
	for (size_t i = 0; i < 4; i++)
	{
		mClearColor.float32[i] = 0.0f;
	}
	mClearDepthStencil.depth = 1.0f;
	mClearDepthStencil.stencil = 0;
}

FVulkanContext::~FVulkanContext()
{
}

void FVulkanContext::Init()
{
	mCurPresentMode = mTargetPresentMode;
	mSwapchain.CreateSwapchain(mCurPresentMode);
	InitFrameData(mFrameData, REV_VK_FRAME_OVERLAP);
	CreateImmediateData();
}

void FVulkanContext::Cleanup()
{
	mGraphicsPipelineCache.ClearAll();

	vkDestroyCommandPool(FVulkanDynamicRHI::GetDevice(), mImmCmdPool, nullptr);
	vkDestroyFence(FVulkanDynamicRHI::GetDevice(), mImmFence, nullptr);

	vkDeviceWaitIdle(FVulkanDynamicRHI::GetDevice());
	CleanupFrameData(mFrameData, REV_VK_FRAME_OVERLAP);
	mSwapchain.Cleanup();
}

void FVulkanContext::Flush()
{
	vkDeviceWaitIdle(FVulkanDynamicRHI::GetDevice());
}

void FVulkanContext::BeginFrame(bool bClearBackBuffer)
{
	constexpr uint64 kWaitTime = 1000000000;
	auto& FrameData = GetActiveFrameData();
	REV_VK_CHECK(vkWaitForFences(FVulkanDynamicRHI::GetDevice(), 1, &FrameData.Fence, true, kWaitTime));
	REV_VK_CHECK(vkResetFences(FVulkanDynamicRHI::GetDevice(), 1, &FrameData.Fence));

	mSwapchain.NextFrame(kWaitTime, FrameData.SwapchainSemaphore, nullptr);

	mDrawExtent.width = mSwapchain.GetExtent().width;
	mDrawExtent.height = mSwapchain.GetExtent().height;

	FrameData.DescriptorPool.ResetPool(FVulkanDynamicRHI::GetDevice());

	VkCommandBuffer CmdBuffer = FrameData.CmdBuffer;
	REV_VK_CHECK(vkResetCommandBuffer(CmdBuffer, 0));
	VkCommandBufferBeginInfo CmdBufferBeginInfo = FVulkanInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(CmdBuffer, &CmdBufferBeginInfo));

	/*if (bClearBackBuffer)
	{
		FVulkanUtils::TransitionImage(CmdBuffer, GetSwapchainImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		ClearBackBuffer();
		FVulkanUtils::TransitionImage(CmdBuffer, GetSwapchainImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
	else*/
	{
		FVulkanUtils::TransitionImage(CmdBuffer, GetSwapchainImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
	}

}

void FVulkanContext::EndFrame()
{
	//end cmd buffer
	auto& FrameData = GetActiveFrameData();
	VkCommandBuffer CmdBuffer = FrameData.CmdBuffer;

	/*FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	FVkUtils::BlitImage(CmdBuffer, mSwapchain.GetBackImage().Image, mSwapchain.GetImages()[mCurSwapchainImageIndex], mDrawExtent, mSwapchain.GetExtent());
	FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);*/

	FVulkanUtils::TransitionImage(CmdBuffer, GetSwapchainImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);

	REV_VK_CHECK(vkEndCommandBuffer(CmdBuffer));

	//submit
	VkCommandBufferSubmitInfo CmdBufferInfo = FVulkanInit::CmdBufferSubmitInfo(CmdBuffer);
	VkSemaphoreSubmitInfo WaitSemaphoreInfo = FVulkanInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, FrameData.SwapchainSemaphore);
	VkSemaphoreSubmitInfo SignalSemaphoreInfo = FVulkanInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, FrameData.RenderSemaphore);

	VkSubmitInfo2 SubmitInfo = FVulkanInit::SubmitInfo(&CmdBufferInfo, &SignalSemaphoreInfo, &WaitSemaphoreInfo);

	//submit command buffer to the queue and execute it.
	//Fence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(FVulkanDynamicRHI::GetQueue(VQK_Graphics), 1, &SubmitInfo, FrameData.Fence));

	mFrameState.Reset();
}

void FVulkanContext::PresentFrame()
{
	auto& FrameData = GetActiveFrameData();
	uint32 SwapchainImageIndex = mSwapchain.GetCurrentTextureIndex();


	//present
	VkPresentInfoKHR PresentInfo{};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = nullptr;
	PresentInfo.pSwapchains = &mSwapchain.GetSwapchain();
	PresentInfo.swapchainCount = 1;
	PresentInfo.pWaitSemaphores = &FrameData.RenderSemaphore;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pImageIndices = &SwapchainImageIndex;
	auto PresentRes = vkQueuePresentKHR(FVulkanDynamicRHI::GetQueue(VQK_Graphics), &PresentInfo);

	if (PresentRes == VK_ERROR_OUT_OF_DATE_KHR || PresentRes == VK_SUBOPTIMAL_KHR || mCurPresentMode != mTargetPresentMode)
	{
		vkDeviceWaitIdle(FVulkanDynamicRHI::GetDevice());
		mSwapchain.Cleanup();
		mCurPresentMode = mTargetPresentMode;
		mSwapchain.CreateSwapchain(mCurPresentMode);
	}
	else if (PresentRes != VK_SUCCESS)
	{
		throw std::runtime_error("[FVkContext] Failed to present swap chain image!");
	}

	mFrameDataIndex = (mFrameDataIndex + 1) % REV_VK_FRAME_OVERLAP;

}

void FVulkanContext::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func)
{
	REV_VK_CHECK(vkResetFences(FVulkanDynamicRHI::GetDevice(), 1, &mImmFence));
	REV_VK_CHECK(vkResetCommandBuffer(mImmCmdBuffer, 0));

	VkCommandBufferBeginInfo BeginInfo = FVulkanInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(mImmCmdBuffer, &BeginInfo));

	Func(mImmCmdBuffer);

	REV_VK_CHECK(vkEndCommandBuffer(mImmCmdBuffer));
	VkCommandBufferSubmitInfo CmdBufferSubmitInfo = FVulkanInit::CmdBufferSubmitInfo(mImmCmdBuffer);
	VkSubmitInfo2 SubmitInfo = FVulkanInit::SubmitInfo(&CmdBufferSubmitInfo, nullptr, nullptr);

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(FVulkanDynamicRHI::GetQueue(VQK_Graphics), 1, &SubmitInfo, mImmFence));
	REV_VK_CHECK(vkWaitForFences(FVulkanDynamicRHI::GetDevice(), 1, &mImmFence, true, 9999999999));
}

void FVulkanContext::RHISetVSync(bool bEnable)
{
	mTargetPresentMode = bEnable ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
}

void FVulkanContext::RHISetViewport(uint32 InX, uint32 InY, uint32 InWidth, uint32 InHeight)
{
	mViewport.x = InX;
	mViewport.y = InY;
	//mViewport.y = InY + InHeight;
	mViewport.width = InWidth;
	mViewport.height = InHeight;
	//mViewport.height = InHeight * -1.0f; //flip viewport to use opengl coordinates
	mViewport.minDepth = 0.0f;
	mViewport.maxDepth = 1.0f;

	mScissor.offset.x = InX;
	mScissor.offset.y = InY;
	mScissor.extent.width = InWidth;
	mScissor.extent.height = InHeight;
}

void FVulkanContext::RHISetClearColor(const Math::FLinearColor& InColor)
{
	for (size_t i = 0; i < 4; i++)
	{
		mClearColor.float32[i] = InColor[i];
	}
}

void FVulkanContext::RHISetClearDepthStencil(float InDepth, uint32 InStencil)
{
	mClearDepthStencil.depth = InDepth;
	mClearDepthStencil.stencil = InStencil;
}

void FVulkanContext::RHIClearBackBuffer()
{
	mSwapchain.GetCurrentTexture()->ClearContent(this, 0, 1, 0, 1);
}

void FVulkanContext::RHIUpdateTexture(FRHITexture* InTexture, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
	if(!InTexture) return;
	FVulkanTexture::Cast(InTexture)->UpdateContent(this, InContent, InSize, InMipLevel, InArrayIndex);
}

void FVulkanContext::RHIClearTexture(FRHITexture* InTexture, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount)
{
	if (!InTexture) return;
	FVulkanTexture::Cast(InTexture)->ClearContent(this, InMipLevel, InMipCount, InArrayIndex, InArrayCount);
}

void FVulkanContext::RHIUpdateBufferData(FRHIBuffer* Buffer, const void* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= Buffer->GetSize());
	FVulkanUtils::ImmediateUploadBuffer(this, (VkBuffer)Buffer->GetNativeHandle(), Content, Size, Offset);
}

void FVulkanContext::RHIBeginRenderPass(FRHIRenderPass* InRenderPass)
{
	FVulkanRenderPass* RenderPass = static_cast<FVulkanRenderPass*>(InRenderPass);
	if(!RenderPass)
		return;
	mFrameState.CurrentPass = RenderPass;
	RenderPass->PrepareForDraw();

	VkRenderPassBeginInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassInfo.pNext = NULL;
	RenderPassInfo.renderPass = (VkRenderPass)RenderPass->GetNativeHandle();
	RenderPassInfo.framebuffer = (VkFramebuffer)RenderPass->GetFramebuffer();
	RenderPassInfo.renderArea.offset.x = 0;
	RenderPassInfo.renderArea.offset.y = 0;
	RenderPassInfo.renderArea.extent.width = RenderPass->GetFrameWidth() > 0 ? RenderPass->GetFrameWidth() : mSwapchain.GetExtent().width;
	RenderPassInfo.renderArea.extent.height = RenderPass->GetFrameHeight() > 0 ? RenderPass->GetFrameHeight() : mSwapchain.GetExtent().height;
	RenderPassInfo.clearValueCount = RenderPass->GetNumAttachments();
	RenderPassInfo.pClearValues = RenderPass->GetClearValues();

	VkSubpassBeginInfo SubpassBeginInfo{};
	SubpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
	SubpassBeginInfo.pNext = NULL;
	SubpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;

	vkCmdBeginRenderPass2(GetActiveCmdBuffer(), &RenderPassInfo, &SubpassBeginInfo);
	vkCmdSetViewport(GetActiveCmdBuffer(), 0, 1, &mViewport);
	vkCmdSetScissor(GetActiveCmdBuffer(), 0, 1, &mScissor);
}


void FVulkanContext::RHIEndRenderPass(bool bBlitToBack)
{
	if(!mFrameState.CurrentPass)
		return;

	VkSubpassEndInfo SubpassEndInfo{};
	SubpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;
	SubpassEndInfo.pNext = NULL;

	vkCmdEndRenderPass2(GetActiveCmdBuffer(), &SubpassEndInfo);

	const FRHIRenderPassDesc& PassDesc = mFrameState.CurrentPass->GetDesc();

	if (bBlitToBack)
	{
		FRHITexture* ColorTex = PassDesc.ColorRenderTargets[RTA_ColorAttachment0].ColorTarget;
		if (ColorTex)
		{
			FVulkanUtils::TransitionImage(GetActiveCmdBuffer(), (VkImage)ColorTex->GetNativeHandle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
			FVulkanUtils::TransitionImage(GetActiveCmdBuffer(), GetSwapchainImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
			FVulkanUtils::BlitImage(GetActiveCmdBuffer(), (VkImage)ColorTex->GetNativeHandle(), GetSwapchainImage(), mDrawExtent, mSwapchain.GetExtent());
			FVulkanUtils::TransitionImage(GetActiveCmdBuffer(), GetSwapchainImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	for (uint32 i = 0; i < PassDesc.NumColorRenderTargets; i++)
	{
		FVulkanTexture* ColorTex = FVulkanTexture::Cast(PassDesc.ColorRenderTargets[RTA_ColorAttachment0].ColorTarget);
		FVulkanUtils::TransitionImage(GetActiveCmdBuffer(), (VkImage)ColorTex->GetNativeHandle(),
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, ColorTex->GetAspectFlags());
	}
	if (PassDesc.DepthStencilRenderTarget.DepthStencilTarget)
	{
		FVulkanTexture* DepthStencilTex = FVulkanTexture::Cast(PassDesc.DepthStencilRenderTarget.DepthStencilTarget);
		FVulkanUtils::TransitionImage(GetActiveCmdBuffer(), (VkImage)DepthStencilTex->GetNativeHandle(),
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, DepthStencilTex->GetAspectFlags());
	}
}

void FVulkanContext::RHINextSubpass()
{
	VkSubpassBeginInfo SubpassBeginInfo{};
	SubpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
	SubpassBeginInfo.pNext = NULL;
	SubpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;

	VkSubpassEndInfo SubpassEndInfo{};
	SubpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;
	SubpassEndInfo.pNext = NULL;

	vkCmdNextSubpass2(GetActiveCmdBuffer(), &SubpassBeginInfo, &SubpassEndInfo);
}

void FVulkanContext::RHIBindUniformBuffer(uint16 InBinding, FRHIUniformBuffer* InBuffer)
{
	if(!InBuffer) return;
	mFrameState.UniformBuffers[InBinding + GShaderCompileConfig.BufferOffset] = static_cast<FVulkanUniformBuffer*>(InBuffer);
}

void FVulkanContext::RHIBindTexture(uint16 InBinding, FRHITexture* InTexture, FRHISamplerState* InSamplerState)
{
	if (!InTexture) return;

	FVulkanTexture* pTexture = static_cast<FVulkanTexture*>(InTexture);
	FVulkanSamplerState* pSamplerState = static_cast<FVulkanSamplerState*>(InSamplerState);
	mFrameState.Textures[InBinding + GShaderCompileConfig.TextureOffset] = { pTexture, pSamplerState };
}

void FVulkanContext::RHIBindProgram(FRHIShaderProgram* InProgram)
{
	mFrameState.CurrentProgram = static_cast<FVulkanShaderProgram*>(InProgram);
}

void FVulkanContext::RHISetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState)
{
	mFrameState.CurrentState = InState;
}

void FVulkanContext::RHISetVertexStream(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset)
{
	if(!VertexBuffer)
		return;

	REV_CORE_ASSERT(StreamIndex < REV_MAX_VERTEX_ELEMENTS);
	mFrameState.VertexStreams[StreamIndex].Buffer = (VkBuffer)VertexBuffer->GetNativeHandle();
	mFrameState.VertexStreams[StreamIndex].Offset = Offset;
	mFrameState.bVertexStreamsDirty = true;
}

void FVulkanContext::RHIDrawPrimitive(uint32 NumPrimitives, uint32 StartVertex)
{
	if (!mFrameState.ReadyForDraw())
		return;

	FVulkanPipeline* GraphicsPipeline = mGraphicsPipelineCache.GetOrCreatePipeline(mFrameState.CurrentState, mFrameState.CurrentPass, mFrameState.CurrentProgram);
	if (!GraphicsPipeline || !GraphicsPipeline->PipelineLayout)
		return;
	FVulkanPipelineLayout* GraphicsPipelineLayout = GraphicsPipeline->PipelineLayout;
	VkDescriptorSet DescSet = GetDescriptorSet(mFrameState.CurrentProgram, GraphicsPipelineLayout);
	vkCmdBindDescriptorSets(GetActiveCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipelineLayout->PipelineLayout, 0, 1, &DescSet, 0, nullptr);
	vkCmdBindPipeline(GetActiveCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline->Pipeline);

	mFrameState.PrepareForDraw(GetActiveCmdBuffer());

	uint32 NumVertices = ComputeVertexCount(NumPrimitives, mFrameState.CurrentState.PrimitiveTopology);
	vkCmdDraw(GetActiveCmdBuffer(), NumVertices, 1, StartVertex, 0);
}

void FVulkanContext::RHIDrawPrimitiveIndexed(FRHIBuffer* IndexBuffer, uint32 NumPrimitives, uint32 StartIndex, int32 VertexOffset)
{
	if (!mFrameState.ReadyForDraw() || !IndexBuffer)
		return;

	FVulkanPipeline* GraphicsPipeline = mGraphicsPipelineCache.GetOrCreatePipeline(mFrameState.CurrentState, mFrameState.CurrentPass, mFrameState.CurrentProgram);
	if (!GraphicsPipeline || !GraphicsPipeline->PipelineLayout)
		return;
	FVulkanPipelineLayout* GraphicsPipelineLayout = GraphicsPipeline->PipelineLayout;
	VkDescriptorSet DescSet = GetDescriptorSet(mFrameState.CurrentProgram, GraphicsPipelineLayout);
	vkCmdBindDescriptorSets(GetActiveCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipelineLayout->PipelineLayout, 0, 1, &DescSet, 0, nullptr);
	vkCmdBindPipeline(GetActiveCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline->Pipeline);

	mFrameState.PrepareForDraw(GetActiveCmdBuffer());

	FVulkanBuffer* IndexBufferVk = static_cast<FVulkanBuffer*>(IndexBuffer);
	vkCmdBindIndexBuffer(GetActiveCmdBuffer(), (VkBuffer)IndexBufferVk->GetNativeHandle(), 0, IndexBufferVk->GetIndexType());

	uint32 NumVertices = ComputeVertexCount(NumPrimitives, mFrameState.CurrentState.PrimitiveTopology);
	vkCmdDrawIndexed(GetActiveCmdBuffer(), NumVertices, 1, StartIndex, 0, 0);
}

void FVulkanContext::RHIBeginDebugLabel(const char* LabelContext, const Math::FLinearColor& Color)
{
	VkDebugUtilsLabelEXT Label = {};
	Label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	Label.pNext = nullptr;
	Label.pLabelName = LabelContext;
	Label.color[0] = Color[0];
	Label.color[1] = Color[1];
	Label.color[2] = Color[2];
	Label.color[3] = Color[3];
	vkCmdBeginDebugUtilsLabelEXT(GetActiveCmdBuffer(), &Label);
}

void FVulkanContext::RHIEndDebugLabel()
{
	vkCmdEndDebugUtilsLabelEXT(GetActiveCmdBuffer());
}

FVulkanContext* FVulkanContext::Cast(FRHIContext* InContext)
{
	REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);
	return static_cast<FVulkanContext*>(InContext);
}

void FVulkanContext::CreateImmediateData()
{
	VkCommandPoolCreateInfo CmdPoolCreateInfo = FVulkanInit::CmdPoolCreateInfo(FVulkanDynamicRHI::GetQueueFamily(VQK_Graphics), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	REV_VK_CHECK(vkCreateCommandPool(FVulkanDynamicRHI::GetDevice(), &CmdPoolCreateInfo, nullptr, &mImmCmdPool));

	// allocate the command buffer for immediate submits
	VkCommandBufferAllocateInfo cmdAllocInfo = FVulkanInit::CmdBufferAllocateInfo(mImmCmdPool, 1);
	REV_VK_CHECK(vkAllocateCommandBuffers(FVulkanDynamicRHI::GetDevice(), &cmdAllocInfo, &mImmCmdBuffer));

	VkFenceCreateInfo FenceCreateInfo = FVulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	REV_VK_CHECK(vkCreateFence(FVulkanDynamicRHI::GetDevice(), &FenceCreateInfo, nullptr, &mImmFence));
}

VkDescriptorSet FVulkanContext::GetDescriptorSet(const FVulkanShaderProgram* InProgram, const FVulkanPipelineLayout* InLayout)
{
	VkDescriptorSetLayout DescLayout = InLayout->DescriptorSetLayout;

	VkDescriptorSet DescSet;
	VkDescriptorSetAllocateInfo DescAllocateInfo{};
	DescAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescAllocateInfo.pNext = NULL;
	DescAllocateInfo.descriptorPool = GetActiveDescriptorPool().Pool;
	DescAllocateInfo.descriptorSetCount = 1;
	DescAllocateInfo.pSetLayouts = &DescLayout;

	REV_VK_CHECK(vkAllocateDescriptorSets(FVulkanDynamicRHI::GetDevice(), &DescAllocateInfo, &DescSet));


	VkDescriptorBufferInfo BufferInfos[REV_VK_MAX_SHADER_UNIFORM_BUFFERS] = {};
	VkDescriptorImageInfo ImageInfos[REV_VK_MAX_SHADER_UNIFORM_SAMPLERS] = {};
	VkWriteDescriptorSet Writes[REV_VK_MAX_DESCRIPTORSETS] = {};
	uint32 WriteCount = 0;
	uint32 BufferCount = 0;
	uint32 ImageCount = 0;

	for (const FRHIShaderUniform& Uniform : InProgram->GetProgramUniforms())
	{
		uint32 BindingIdx = Uniform.Binding;
		switch (Uniform.Type)
		{
		case EShaderUniformType::Buffer:
		{
			FVulkanUniformBuffer* UniformBuffer = mFrameState.FindUniformBuffer(BindingIdx);
			if (UniformBuffer)
			{
				BufferInfos[BufferCount].buffer = (VkBuffer)UniformBuffer->GetNativeHandle();
				BufferInfos[BufferCount].offset = 0;
				BufferInfos[BufferCount].range = UniformBuffer->GetSize();

				Writes[WriteCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				Writes[WriteCount].pNext = NULL;
				Writes[WriteCount].dstSet = DescSet;
				Writes[WriteCount].dstBinding = BindingIdx;
				Writes[WriteCount].dstArrayElement = 0;
				Writes[WriteCount].descriptorCount = 1;
				Writes[WriteCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				Writes[WriteCount].pImageInfo = NULL;
				Writes[WriteCount].pBufferInfo = &BufferInfos[BufferCount];
				Writes[WriteCount].pTexelBufferView = NULL;

				++WriteCount;
				++BufferCount;
			}
		}
		break;
		case EShaderUniformType::Texture:
		{
			auto TextureAndSamplerState = mFrameState.FindTexture(BindingIdx);
			FVulkanTexture* Texture = TextureAndSamplerState.first;
			FVulkanSamplerState* SamplerState = TextureAndSamplerState.second;
			if (Texture)
			{
				ImageInfos[ImageCount].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				ImageInfos[ImageCount].imageView = Texture->GetImageView();
				ImageInfos[ImageCount].sampler = SamplerState ? SamplerState->Sampler : VK_NULL_HANDLE;
				
				Writes[WriteCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				Writes[WriteCount].pNext = NULL;
				Writes[WriteCount].dstSet = DescSet;
				Writes[WriteCount].dstBinding = BindingIdx;
				Writes[WriteCount].dstArrayElement = 0;
				Writes[WriteCount].descriptorCount = 1;
				Writes[WriteCount].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				Writes[WriteCount].pImageInfo = &ImageInfos[ImageCount];
				Writes[WriteCount].pBufferInfo = NULL;
				Writes[WriteCount].pTexelBufferView = NULL;
				++WriteCount;

				if (SamplerState)
				{
					Writes[WriteCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					Writes[WriteCount].pNext = NULL;
					Writes[WriteCount].dstSet = DescSet;
					Writes[WriteCount].dstBinding = Uniform.SamplerBinding;
					Writes[WriteCount].dstArrayElement = 0;
					Writes[WriteCount].descriptorCount = 1;
					Writes[WriteCount].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
					Writes[WriteCount].pImageInfo = &ImageInfos[ImageCount];
					Writes[WriteCount].pBufferInfo = NULL;
					Writes[WriteCount].pTexelBufferView = NULL;
					++WriteCount;
				}

				++ImageCount;
			}
		}
		default:
			break;
		}
		
	}

	vkUpdateDescriptorSets(FVulkanDynamicRHI::GetDevice(), WriteCount, Writes, 0, NULL);

	return DescSet;
}

}