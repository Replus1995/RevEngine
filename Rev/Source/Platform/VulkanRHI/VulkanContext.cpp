#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanTexture2D.h"
#include "VulkanState.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
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

	FrameData.DescriptorPool.ResetPool(FVulkanDynamicRHI::GetDevice());

	VkCommandBuffer CmdBuffer = FrameData.CmdBuffer;
	REV_VK_CHECK(vkResetCommandBuffer(CmdBuffer, 0));
	VkCommandBufferBeginInfo CmdBufferBeginInfo = FVulkanInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(CmdBuffer, &CmdBufferBeginInfo));

	// The RenderGraph owns ordinary rendering transitions.

}

void FVulkanContext::EndFrame()
{
	//end cmd buffer
	auto& FrameData = GetActiveFrameData();
	VkCommandBuffer CmdBuffer = FrameData.CmdBuffer;

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

void FVulkanContext::RHIClearBackTexture(const Math::FLinearColor& InColor)
{
	VkClearValue ClearValue;
	ClearValue.color.float32[0] = InColor[0];
	ClearValue.color.float32[1] = InColor[1];
	ClearValue.color.float32[2] = InColor[2];
	ClearValue.color.float32[3] = InColor[3];

	auto SwapchainTexture = GetSwapchainTexture();
	FVulkanUtils::ImmediateClearImage(this, SwapchainTexture->GetImage(), SwapchainTexture->GetAspectFlags(), ClearValue, 0, 1, 0, 1);
}

uint32 FVulkanContext::RHIGetFrameWidth()
{
	return mSwapchain.GetExtent().width;
}

uint32 FVulkanContext::RHIGetFrameHeight()
{
	return mSwapchain.GetExtent().height;
}

FRHITexture* FVulkanContext::RHIGetBackTexture()
{
	return mSwapchain.GetCurrentTexture();
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

void FVulkanContext::RHIBlitTexture(FRHITexture* DstTexture, FRHITexture* SrcTexture)
{
	if (!DstTexture  || !SrcTexture) return;

	FVulkanTexture* DstTextureVk = FVulkanTexture::Cast(DstTexture);
	FVulkanTexture* SrcTextureVk = FVulkanTexture::Cast(SrcTexture);

	DstTextureVk->DoTransition(GetActiveCmdBuffer(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	SrcTextureVk->DoTransition(GetActiveCmdBuffer(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	FVulkanUtils::BlitImage(GetActiveCmdBuffer(), DstTextureVk->GetImage(), DstTextureVk->GetExtent(), SrcTextureVk->GetImage(), SrcTextureVk->GetExtent());
}

void FVulkanContext::RHIBlitToBackTexture(FRHITexture* SrcTexture)
{
	RHIBlitTexture(GetSwapchainTexture(), SrcTexture);
}

void FVulkanContext::RHIUpdateBufferData(FRHIBuffer* Buffer, const void* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= Buffer->GetSize());
	FVulkanUtils::ImmediateUploadBuffer(this, (VkBuffer)Buffer->GetNativeHandle(), Content, Size, Offset);
}

namespace
{
struct FVulkanAccessInfo { VkPipelineStageFlags2 Stages; VkAccessFlags2 Access; VkImageLayout Layout; };

FVulkanAccessInfo TranslateAccess(ERHIAccess Access, bool bDepth)
{
	switch (Access)
	{
	case ERHIAccess::Present: return { VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };
	case ERHIAccess::ColorAttachment: return { VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	case ERHIAccess::DepthStencilWrite: return { VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
	case ERHIAccess::DepthStencilRead: return { VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };
	case ERHIAccess::ShaderRead: return { VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT, bDepth ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	case ERHIAccess::CopySrc: return { VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL };
	case ERHIAccess::CopyDst: return { VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL };
	case ERHIAccess::VertexBuffer: return { VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT, VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
	case ERHIAccess::IndexBuffer: return { VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT, VK_ACCESS_2_INDEX_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
	case ERHIAccess::UniformRead: return { VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_UNIFORM_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED };
	default: return { VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE, VK_IMAGE_LAYOUT_UNDEFINED };
	}
}

VkImageAspectFlags TranslateAspect(const FVulkanTexture& Texture, ERHITextureAspect Aspect)
{
	if (Aspect == ERHITextureAspect::Auto) return Texture.GetAspectFlags();
	VkImageAspectFlags Result = 0;
	if (EnumHasAnyFlags(Aspect, ERHITextureAspect::Color)) Result |= VK_IMAGE_ASPECT_COLOR_BIT;
	if (EnumHasAnyFlags(Aspect, ERHITextureAspect::Depth)) Result |= VK_IMAGE_ASPECT_DEPTH_BIT;
	if (EnumHasAnyFlags(Aspect, ERHITextureAspect::Stencil)) Result |= VK_IMAGE_ASPECT_STENCIL_BIT;
	return Result;
}
}

void FVulkanContext::RHITransition(std::span<const FRHITextureBarrier> InTextureBarriers, std::span<const FRHIBufferBarrier> InBufferBarriers)
{
	std::vector<VkImageMemoryBarrier2> Images;
	std::vector<VkBufferMemoryBarrier2> Buffers;
	for (const FRHITextureBarrier& Barrier : InTextureBarriers)
	{
		if (!Barrier.Texture || Barrier.Before == Barrier.After) continue;
		FVulkanTexture* Texture = FVulkanTexture::Cast(Barrier.Texture);
		const bool bDepth = FPixelFormatInfo::HasDepth(Texture->GetFormat());
		const FVulkanAccessInfo Src = TranslateAccess(Barrier.Before, bDepth);
		const FVulkanAccessInfo Dst = TranslateAccess(Barrier.After, bDepth);
		VkImageMemoryBarrier2 Out{};
		Out.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		Out.srcStageMask = Src.Stages; Out.srcAccessMask = Src.Access;
		Out.dstStageMask = Dst.Stages; Out.dstAccessMask = Dst.Access;
		Out.oldLayout = (Barrier.Before == ERHIAccess::Unknown || Texture->GetImageLayout() == VK_IMAGE_LAYOUT_UNDEFINED) ? Texture->GetImageLayout() : Src.Layout;
		Out.newLayout = Dst.Layout;
		Out.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; Out.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Out.image = Texture->GetImage();
		Out.subresourceRange.aspectMask = TranslateAspect(*Texture, Barrier.Range.Aspect);
		Out.subresourceRange.baseMipLevel = Barrier.Range.BaseMip;
		Out.subresourceRange.levelCount = Barrier.Range.NumMips ? Barrier.Range.NumMips : Texture->GetDesc().NumMips - Barrier.Range.BaseMip;
		Out.subresourceRange.baseArrayLayer = Barrier.Range.BaseLayer;
		const uint32 TotalLayers = Texture->GetDesc().Dimension == ETextureDimension::TextureCube ? 6u : Texture->GetDesc().ArraySize;
		Out.subresourceRange.layerCount = Barrier.Range.NumLayers ? Barrier.Range.NumLayers : TotalLayers - Barrier.Range.BaseLayer;
		Images.push_back(Out);
		Texture->SetImageLayout(Dst.Layout);
	}
	for (const FRHIBufferBarrier& Barrier : InBufferBarriers)
	{
		if (!Barrier.Buffer || Barrier.Before == Barrier.After) continue;
		const FVulkanAccessInfo Src = TranslateAccess(Barrier.Before, false);
		const FVulkanAccessInfo Dst = TranslateAccess(Barrier.After, false);
		VkBufferMemoryBarrier2 Out{};
		Out.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
		Out.srcStageMask = Src.Stages; Out.srcAccessMask = Src.Access;
		Out.dstStageMask = Dst.Stages; Out.dstAccessMask = Dst.Access;
		Out.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; Out.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Out.buffer = (VkBuffer)Barrier.Buffer->GetNativeHandle(); Out.size = VK_WHOLE_SIZE;
		Buffers.push_back(Out);
	}
	if (Images.empty() && Buffers.empty()) return;
	VkDependencyInfo Dependency{};
	Dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	Dependency.imageMemoryBarrierCount = uint32(Images.size()); Dependency.pImageMemoryBarriers = Images.data();
	Dependency.bufferMemoryBarrierCount = uint32(Buffers.size()); Dependency.pBufferMemoryBarriers = Buffers.data();
	vkCmdPipelineBarrier2(GetActiveCmdBuffer(), &Dependency);
}

void FVulkanContext::RHIBeginRendering(const FRHIRenderingInfo& InInfo)
{
	REV_CORE_ASSERT(!mFrameState.bRendering);
	std::array<VkRenderingAttachmentInfo, REV_MAX_RENDER_TARGETS> Colors{};
	for (uint32 Index = 0; Index < InInfo.NumColorAttachments; ++Index)
	{
		const FRHIRenderingAttachment& Source = InInfo.ColorAttachments[Index];
		FVulkanTexture* Texture = FVulkanTexture::Cast(Source.Texture);
		VkRenderingAttachmentInfo& Target = Colors[Index];
		Target.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		Target.imageView = Texture->GetImageView(Source.Subresource);
		Target.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		Target.loadOp = FVulkanEnum::Translate(Source.LoadAction); Target.storeOp = FVulkanEnum::Translate(Source.StoreAction);
		Target.clearValue = Texture->GetClearValue();
		if (Source.ResolveTexture)
		{
			Target.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
			Target.resolveImageView = FVulkanTexture::Cast(Source.ResolveTexture)->GetImageView(Source.Subresource);
			Target.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}
	auto MakeDepthStencil = [](const FRHIRenderingAttachment& Source, bool bReadOnly)
	{
		VkRenderingAttachmentInfo Result{};
		if (!Source.Texture) return Result;
		FVulkanTexture* Texture = FVulkanTexture::Cast(Source.Texture);
		Result.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		Result.imageView = Texture->GetImageView(Source.Subresource);
		Result.imageLayout = bReadOnly ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		Result.loadOp = FVulkanEnum::Translate(Source.LoadAction); Result.storeOp = FVulkanEnum::Translate(Source.StoreAction);
		Result.clearValue = Texture->GetClearValue();
		return Result;
	};
	VkRenderingAttachmentInfo Depth = MakeDepthStencil(InInfo.DepthAttachment, InInfo.bDepthReadOnly);
	VkRenderingAttachmentInfo Stencil = MakeDepthStencil(InInfo.StencilAttachment, InInfo.bStencilReadOnly);
	VkRenderingInfo Rendering{};
	Rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	Rendering.renderArea.extent = { InInfo.Width, InInfo.Height }; Rendering.layerCount = 1;
	Rendering.colorAttachmentCount = InInfo.NumColorAttachments; Rendering.pColorAttachments = Colors.data();
	Rendering.pDepthAttachment = InInfo.DepthAttachment.Texture ? &Depth : nullptr;
	Rendering.pStencilAttachment = InInfo.StencilAttachment.Texture ? &Stencil : nullptr;
	vkCmdBeginRendering(GetActiveCmdBuffer(), &Rendering);
	mFrameState.bRendering = true;
	vkCmdSetViewport(GetActiveCmdBuffer(), 0, 1, &mViewport);
	vkCmdSetScissor(GetActiveCmdBuffer(), 0, 1, &mScissor);
}

void FVulkanContext::RHIEndRendering()
{
	if (!mFrameState.bRendering) return;
	vkCmdEndRendering(GetActiveCmdBuffer());
	mFrameState.bRendering = false;
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

void FVulkanContext::RHISetRenderTargetLayout(const FRHIRenderTargetLayout& InLayout)
{
	mFrameState.CurrentRenderTargetLayout = InLayout;
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

	FVulkanPipeline* GraphicsPipeline = mGraphicsPipelineCache.GetOrCreatePipeline(mFrameState.CurrentState, mFrameState.CurrentRenderTargetLayout, mFrameState.CurrentProgram);
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

	FVulkanPipeline* GraphicsPipeline = mGraphicsPipelineCache.GetOrCreatePipeline(mFrameState.CurrentState, mFrameState.CurrentRenderTargetLayout, mFrameState.CurrentProgram);
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

FVulkanContext* FVulkanContext::Cast(IRHIContext* InContext)
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
				ImageInfos[ImageCount].imageLayout = FPixelFormatInfo::HasDepth(Texture->GetFormat()) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				FRHITextureSubresourceRange SampleRange;
				if (FPixelFormatInfo::HasDepth(Texture->GetFormat())) SampleRange.Aspect = ERHITextureAspect::Depth;
				ImageInfos[ImageCount].imageView = Texture->GetImageView(SampleRange);
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
