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
	// A newly-created swapchain image has not been presented yet, so its tracked
	// layout remains UNDEFINED until RenderGraph transitions it for the first use.

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
	else
	{
		FVulkanTexture* BackTexture = mSwapchain.GetCurrentTexture();
		BackTexture->SetImageLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		BackTexture->SetSubresourceLayout(0, 0, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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

	vkCmdSetViewport(GetActiveCmdBuffer(), 0, 1, &mViewport);
	vkCmdSetScissor(GetActiveCmdBuffer(), 0, 1, &mScissor);
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
	REV_CORE_ASSERT(InTexture->GetDesc().NumSamples == 1, "MSAA textures cannot be uploaded from CPU memory");
	FVulkanTexture::Cast(InTexture)->UpdateContent(this, InContent, InSize, InMipLevel, InArrayIndex);
}

void FVulkanContext::RHIUpdateTexture(FRHITexture* InTexture, const FRHITextureUpdateDesc& InDesc)
{
	if (!InTexture || !InDesc.Data || InDesc.DataSize == 0) return;
	const FRHITextureDesc& TextureDesc = InTexture->GetDesc();
	REV_CORE_ASSERT(TextureDesc.NumSamples == 1, "MSAA textures cannot be uploaded from CPU memory");
	REV_CORE_ASSERT(InDesc.Subresource.MipLevel < TextureDesc.NumMips);
	REV_CORE_ASSERT(InDesc.Subresource.ArrayLayer < TextureDesc.GetPhysicalLayerCount());
	const Math::FVector3 MipExtent = TextureDesc.GetMipExtent(InDesc.Subresource.MipLevel);
	const uint32 Width = InDesc.Region.Width ? InDesc.Region.Width : uint32(MipExtent.X) - InDesc.Region.X;
	const uint32 Height = InDesc.Region.Height ? InDesc.Region.Height : uint32(MipExtent.Y) - InDesc.Region.Y;
	const uint32 Depth = InDesc.Region.Depth ? InDesc.Region.Depth : uint32(MipExtent.Z) - InDesc.Region.Z;
	REV_CORE_ASSERT(Width > 0 && Height > 0 && Depth > 0);
	REV_CORE_ASSERT(InDesc.Region.X + Width <= uint32(MipExtent.X) && InDesc.Region.Y + Height <= uint32(MipExtent.Y) && InDesc.Region.Z + Depth <= uint32(MipExtent.Z));
	const FPixelFormatInfo& FormatInfo = GPixelFormats[TextureDesc.Format];
	REV_CORE_ASSERT(FormatInfo.BlockSizeX == 1 && FormatInfo.BlockSizeY == 1 && FormatInfo.BlockSizeZ == 1, "Compressed texture updates are not supported");
	const uint32 TightRowPitch = Width * FormatInfo.BlockBytes;
	const uint32 RowPitch = InDesc.RowPitch ? InDesc.RowPitch : TightRowPitch;
	const uint32 SlicePitch = InDesc.SlicePitch ? InDesc.SlicePitch : RowPitch * Height;
	REV_CORE_ASSERT(RowPitch >= TightRowPitch && RowPitch % FormatInfo.BlockBytes == 0);
	REV_CORE_ASSERT(SlicePitch >= RowPitch * Height && SlicePitch % RowPitch == 0);
	const uint64 RequiredSize = uint64(SlicePitch) * (Depth - 1) + uint64(RowPitch) * (Height - 1) + TightRowPitch;
	REV_CORE_ASSERT(InDesc.DataSize >= RequiredSize);
	REV_CORE_ASSERT(InDesc.DataSize <= 0xFFFFFFFFull);
	FVulkanTexture* Texture = FVulkanTexture::Cast(InTexture);
	FVulkanUtils::ImmediateUploadImage(this, Texture->GetImage(), Texture->GetAspectFlags(), { Width, Height, Depth }, InDesc.Data, uint32(InDesc.DataSize), uint8(InDesc.Subresource.MipLevel), InDesc.Subresource.ArrayLayer, 1, Texture->GetSubresourceLayout(uint8(InDesc.Subresource.MipLevel), InDesc.Subresource.ArrayLayer), { int32(InDesc.Region.X), int32(InDesc.Region.Y), int32(InDesc.Region.Z) }, RowPitch / FormatInfo.BlockBytes, SlicePitch / RowPitch);
	Texture->SetSubresourceLayout(uint8(InDesc.Subresource.MipLevel), InDesc.Subresource.ArrayLayer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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

namespace VulkanContextPrivate
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

}

using namespace VulkanContextPrivate;

void FVulkanContext::RHICopyTexture(FRHITexture* DstTexture, FRHITexture* SrcTexture, const FRHITextureCopyDesc& InDesc)
{
	if (!DstTexture || !SrcTexture) return;
	FVulkanTexture* Dst = FVulkanTexture::Cast(DstTexture);
	FVulkanTexture* Src = FVulkanTexture::Cast(SrcTexture);
	const FRHITextureDesc& SrcDesc = SrcTexture->GetDesc();
	const FRHITextureDesc& DstDesc = DstTexture->GetDesc();
	REV_CORE_ASSERT(Src->GetPlatformFormat() == Dst->GetPlatformFormat() && Src->GetAspectFlags() == Dst->GetAspectFlags(), "Texture copy requires matching formats and aspects");
	REV_CORE_ASSERT(InDesc.LayerCount > 0);
	REV_CORE_ASSERT(InDesc.SourceSubresource.MipLevel < SrcDesc.NumMips && InDesc.DestinationSubresource.MipLevel < DstDesc.NumMips);
	REV_CORE_ASSERT(InDesc.SourceSubresource.ArrayLayer + InDesc.LayerCount <= SrcDesc.GetPhysicalLayerCount());
	REV_CORE_ASSERT(InDesc.DestinationSubresource.ArrayLayer + InDesc.LayerCount <= DstDesc.GetPhysicalLayerCount());
	const Math::FVector3 SourceMip = SrcDesc.GetMipExtent(InDesc.SourceSubresource.MipLevel);
	const uint32 Width = InDesc.SourceRegion.Width ? InDesc.SourceRegion.Width : uint32(SourceMip.X);
	const uint32 Height = InDesc.SourceRegion.Height ? InDesc.SourceRegion.Height : uint32(SourceMip.Y);
	const uint32 Depth = InDesc.SourceRegion.Depth ? InDesc.SourceRegion.Depth : uint32(SourceMip.Z);
	const Math::FVector3 DestinationMip = DstDesc.GetMipExtent(InDesc.DestinationSubresource.MipLevel);
	REV_CORE_ASSERT(InDesc.SourceRegion.X + Width <= uint32(SourceMip.X) && InDesc.SourceRegion.Y + Height <= uint32(SourceMip.Y) && InDesc.SourceRegion.Z + Depth <= uint32(SourceMip.Z));
	REV_CORE_ASSERT(InDesc.DestinationRegion.X + Width <= uint32(DestinationMip.X) && InDesc.DestinationRegion.Y + Height <= uint32(DestinationMip.Y) && InDesc.DestinationRegion.Z + Depth <= uint32(DestinationMip.Z));
	ImmediateSubmit([&](VkCommandBuffer Cmd) {
		auto TransitionSubresource = [&](FVulkanTexture* Texture, uint16 Mip, uint16 Layer, uint16 LayerCount, VkImageLayout Layout, VkAccessFlags2 Access) {
			VkImageMemoryBarrier2 Barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
			Barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; Barrier.srcAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
			Barrier.dstStageMask = Layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ? VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT : VK_PIPELINE_STAGE_2_TRANSFER_BIT; Barrier.dstAccessMask = Access;
			Barrier.oldLayout = Texture->GetSubresourceLayout(uint8(Mip), Layer); Barrier.newLayout = Layout; Barrier.image = Texture->GetImage();
			Barrier.subresourceRange = { Texture->GetAspectFlags(), Mip, 1, Layer, LayerCount };
			VkDependencyInfo Dependency{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO }; Dependency.imageMemoryBarrierCount = 1; Dependency.pImageMemoryBarriers = &Barrier; vkCmdPipelineBarrier2(Cmd, &Dependency);
			for (uint16 Index = 0; Index < LayerCount; ++Index) Texture->SetSubresourceLayout(uint8(Mip), Layer + Index, Layout);
		};
		TransitionSubresource(Src, InDesc.SourceSubresource.MipLevel, InDesc.SourceSubresource.ArrayLayer, InDesc.LayerCount, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_2_TRANSFER_READ_BIT);
		TransitionSubresource(Dst, InDesc.DestinationSubresource.MipLevel, InDesc.DestinationSubresource.ArrayLayer, InDesc.LayerCount, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_2_TRANSFER_WRITE_BIT);
		VkImageCopy Region{};
		Region.srcSubresource = { Src->GetAspectFlags(), InDesc.SourceSubresource.MipLevel, InDesc.SourceSubresource.ArrayLayer, InDesc.LayerCount };
		Region.dstSubresource = { Dst->GetAspectFlags(), InDesc.DestinationSubresource.MipLevel, InDesc.DestinationSubresource.ArrayLayer, InDesc.LayerCount };
		Region.srcOffset = { int32(InDesc.SourceRegion.X), int32(InDesc.SourceRegion.Y), int32(InDesc.SourceRegion.Z) };
		Region.dstOffset = { int32(InDesc.DestinationRegion.X), int32(InDesc.DestinationRegion.Y), int32(InDesc.DestinationRegion.Z) };
		Region.extent = { Width, Height, Depth };
		vkCmdCopyImage(Cmd, Src->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, Dst->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);
		TransitionSubresource(Src, InDesc.SourceSubresource.MipLevel, InDesc.SourceSubresource.ArrayLayer, InDesc.LayerCount, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
		TransitionSubresource(Dst, InDesc.DestinationSubresource.MipLevel, InDesc.DestinationSubresource.ArrayLayer, InDesc.LayerCount, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
	});
}

bool FVulkanContext::RHIGenerateMips(FRHITexture* InTexture)
{
	if (!InTexture) return false;
	FVulkanTexture* Texture = FVulkanTexture::Cast(InTexture);
	const FRHITextureDesc& Desc = Texture->GetDesc();
	if (Desc.NumMips < 2 || Desc.NumSamples != 1 || FPixelFormatInfo::HasDepth(Desc.Format) || FPixelFormatInfo::HasStencil(Desc.Format) ||
		!EnumHasAllFlags(Desc.Flags, ETextureCreateFlags::TransferSrc | ETextureCreateFlags::TransferDst)) return false;
	VkFormatProperties Properties{};
	vkGetPhysicalDeviceFormatProperties(FVulkanDynamicRHI::GetPhysicalDevice(), Texture->GetPlatformFormat(), &Properties);
	const VkFormatFeatureFlags Required = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
	if ((Properties.optimalTilingFeatures & Required) != Required) return false;
	const uint32 Layers = Desc.Dimension == ETextureDimension::Texture3D ? 1 : Desc.GetPhysicalLayerCount();
	ImmediateSubmit([&](VkCommandBuffer Cmd) {
		for (uint32 Mip = 1; Mip < Desc.NumMips; ++Mip)
		{
			const VkImageLayout PreviousLayout = Texture->GetSubresourceLayout(uint8(Mip - 1), 0);
			const VkImageLayout CurrentLayout = Texture->GetSubresourceLayout(uint8(Mip), 0);
			VkImageMemoryBarrier2 Barriers[2]{};
			for (VkImageMemoryBarrier2& Barrier : Barriers) { Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2; Barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; Barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT; Barrier.image = Texture->GetImage(); Barrier.subresourceRange.aspectMask = Texture->GetAspectFlags(); Barrier.subresourceRange.baseArrayLayer = 0; Barrier.subresourceRange.layerCount = Layers; }
			Barriers[0].srcAccessMask = PreviousLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_ACCESS_2_NONE : VK_ACCESS_2_SHADER_SAMPLED_READ_BIT; Barriers[0].dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT; Barriers[0].oldLayout = PreviousLayout; Barriers[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; Barriers[0].subresourceRange.baseMipLevel = Mip - 1; Barriers[0].subresourceRange.levelCount = 1;
			Barriers[1].srcAccessMask = CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_ACCESS_2_NONE : VK_ACCESS_2_SHADER_SAMPLED_READ_BIT; Barriers[1].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT; Barriers[1].oldLayout = CurrentLayout; Barriers[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; Barriers[1].subresourceRange.baseMipLevel = Mip; Barriers[1].subresourceRange.levelCount = 1;
			VkDependencyInfo Dependency{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO }; Dependency.imageMemoryBarrierCount = 2; Dependency.pImageMemoryBarriers = Barriers; vkCmdPipelineBarrier2(Cmd, &Dependency);
			const Math::FVector3 Source = Desc.GetMipExtent(uint8(Mip - 1));
			const Math::FVector3 Destination = Desc.GetMipExtent(uint8(Mip));
			VkImageBlit Blit{};
			Blit.srcSubresource = { Texture->GetAspectFlags(), Mip - 1, 0, Layers }; Blit.dstSubresource = { Texture->GetAspectFlags(), Mip, 0, Layers };
			Blit.srcOffsets[1] = { int32(Source.X), int32(Source.Y), int32(Source.Z) }; Blit.dstOffsets[1] = { int32(Destination.X), int32(Destination.Y), int32(Destination.Z) };
			vkCmdBlitImage(Cmd, Texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, Texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Blit, VK_FILTER_LINEAR);
			VkImageMemoryBarrier2 Complete{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
			Complete.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT; Complete.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT; Complete.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; Complete.dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
			Complete.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; Complete.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; Complete.image = Texture->GetImage(); Complete.subresourceRange = { Texture->GetAspectFlags(), Mip - 1, 1, 0, Layers };
			VkDependencyInfo CompleteDependency{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO }; CompleteDependency.imageMemoryBarrierCount = 1; CompleteDependency.pImageMemoryBarriers = &Complete; vkCmdPipelineBarrier2(Cmd, &CompleteDependency);
			for (uint16 Layer = 0; Layer < Layers; ++Layer) Texture->SetSubresourceLayout(uint8(Mip - 1), Layer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		VkImageMemoryBarrier2 Complete{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
		Complete.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT; Complete.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT; Complete.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; Complete.dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
		Complete.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; Complete.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; Complete.image = Texture->GetImage(); Complete.subresourceRange = { Texture->GetAspectFlags(), uint32(Desc.NumMips - 1), 1, 0, Layers };
		VkDependencyInfo CompleteDependency{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO }; CompleteDependency.imageMemoryBarrierCount = 1; CompleteDependency.pImageMemoryBarriers = &Complete; vkCmdPipelineBarrier2(Cmd, &CompleteDependency);
		for (uint16 Layer = 0; Layer < Layers; ++Layer) Texture->SetSubresourceLayout(uint8(Desc.NumMips - 1), Layer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		Texture->SetImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	});
	return true;
}

namespace VulkanContextPrivate
{
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

using namespace VulkanContextPrivate;

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
		const VkImageLayout ActualLayout = Texture->GetSubresourceLayout(Barrier.Range.BaseMip, Barrier.Range.BaseLayer);
		Out.oldLayout = (Barrier.Before == ERHIAccess::Unknown || ActualLayout == VK_IMAGE_LAYOUT_UNDEFINED) ? ActualLayout : Src.Layout;
		Out.newLayout = Dst.Layout;
		Out.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; Out.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Out.image = Texture->GetImage();
		Out.subresourceRange.aspectMask = TranslateAspect(*Texture, Barrier.Range.Aspect);
		Out.subresourceRange.baseMipLevel = Barrier.Range.BaseMip;
		Out.subresourceRange.levelCount = Barrier.Range.NumMips ? Barrier.Range.NumMips : Texture->GetDesc().NumMips - Barrier.Range.BaseMip;
		Out.subresourceRange.baseArrayLayer = Barrier.Range.BaseLayer;
		const uint32 TotalLayers = Texture->GetDesc().GetPhysicalLayerCount();
		Out.subresourceRange.layerCount = Barrier.Range.NumLayers ? Barrier.Range.NumLayers : TotalLayers - Barrier.Range.BaseLayer;
		if (Barrier.Before != ERHIAccess::Unknown)
		{
			for (uint32 Layer = Out.subresourceRange.baseArrayLayer; Layer < Out.subresourceRange.baseArrayLayer + Out.subresourceRange.layerCount; ++Layer)
				for (uint32 Mip = Out.subresourceRange.baseMipLevel; Mip < Out.subresourceRange.baseMipLevel + Out.subresourceRange.levelCount; ++Mip)
				{
					const VkImageLayout KnownLayout = Texture->GetSubresourceLayout(uint8(Mip), uint16(Layer));
					REV_CORE_ASSERT(KnownLayout == VK_IMAGE_LAYOUT_UNDEFINED || KnownLayout == Src.Layout, "Texture barrier before-access does not match the tracked subresource state");
				}
		}
		Images.push_back(Out);
		for (uint32 Layer = Out.subresourceRange.baseArrayLayer; Layer < Out.subresourceRange.baseArrayLayer + Out.subresourceRange.layerCount; ++Layer)
			for (uint32 Mip = Out.subresourceRange.baseMipLevel; Mip < Out.subresourceRange.baseMipLevel + Out.subresourceRange.levelCount; ++Mip)
				Texture->SetSubresourceLayout(uint8(Mip), uint16(Layer), Dst.Layout);
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
	uint32 RenderingLayerCount = 1;
	auto GetLayerCount = [](const FRHIRenderingAttachment& Attachment) {
		if (!Attachment.Texture) return 1u;
		const FRHITextureDesc& Desc = Attachment.Texture->GetDesc();
		return Attachment.Subresource.NumLayers ? uint32(Attachment.Subresource.NumLayers) : uint32(Desc.GetPhysicalLayerCount() - Attachment.Subresource.BaseLayer);
	};
	std::array<VkRenderingAttachmentInfo, REV_MAX_RENDER_TARGETS> Colors{};
	for (uint32 Index = 0; Index < InInfo.NumColorAttachments; ++Index)
	{
		const FRHIRenderingAttachment& Source = InInfo.ColorAttachments[Index];
		FVulkanTexture* Texture = FVulkanTexture::Cast(Source.Texture);
		VkRenderingAttachmentInfo& Target = Colors[Index];
		Target.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		Target.imageView = Texture->GetImageView({ Source.Subresource, ERHITextureViewType::Attachment });
		Target.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		Target.loadOp = FVulkanEnum::Translate(Source.LoadAction); Target.storeOp = FVulkanEnum::Translate(Source.StoreAction);
		Target.clearValue = Texture->GetClearValue();
		RenderingLayerCount = std::max(RenderingLayerCount, GetLayerCount(Source));
		if (Source.ResolveTexture)
		{
			Target.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
			Target.resolveImageView = FVulkanTexture::Cast(Source.ResolveTexture)->GetImageView({ Source.Subresource, ERHITextureViewType::Attachment });
			Target.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}
	auto MakeDepthStencil = [](const FRHIRenderingAttachment& Source, bool bReadOnly)
	{
		VkRenderingAttachmentInfo Result{};
		if (!Source.Texture) return Result;
		FVulkanTexture* Texture = FVulkanTexture::Cast(Source.Texture);
		Result.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		Result.imageView = Texture->GetImageView({ Source.Subresource, ERHITextureViewType::Attachment });
		Result.imageLayout = bReadOnly ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		Result.loadOp = FVulkanEnum::Translate(Source.LoadAction); Result.storeOp = FVulkanEnum::Translate(Source.StoreAction);
		Result.clearValue = Texture->GetClearValue();
		return Result;
	};
	VkRenderingAttachmentInfo Depth = MakeDepthStencil(InInfo.DepthAttachment, InInfo.bDepthReadOnly);
	VkRenderingAttachmentInfo Stencil = MakeDepthStencil(InInfo.StencilAttachment, InInfo.bStencilReadOnly);
	if (InInfo.DepthAttachment.Texture) RenderingLayerCount = std::max(RenderingLayerCount, GetLayerCount(InInfo.DepthAttachment));
	if (InInfo.StencilAttachment.Texture) RenderingLayerCount = std::max(RenderingLayerCount, GetLayerCount(InInfo.StencilAttachment));
	VkRenderingInfo Rendering{};
	Rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	Rendering.renderArea.extent = { InInfo.Width, InInfo.Height }; Rendering.layerCount = RenderingLayerCount;
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
				ImageInfos[ImageCount].imageView = Texture->GetImageView({ SampleRange, ERHITextureViewType::ShaderResource });
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
