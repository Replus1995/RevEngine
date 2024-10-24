#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Core/Deleter.h"
#include <vector>
#include <map>
#include <vulkan/vulkan.h>

#include "Core/VulkanDefines.h"
#include "Core/VulkanInit.h"
#include "VulkanSwapchain.h"
#include "VulkanFrameData.h"
#include "VulkanPipeline.h"
#include "VulkanState.h"

namespace Rev
{

class FVulkanRenderPass;
class FVulkanShaderProgram;
class FVulkanUniformBuffer;
class FVulkanTexture;
class FVulkanSamplerState;
class FVulkanPipelineLayout;
class FVulkanPipeline;

class FVulkanContext : public FRHIContext
{
public:
	FVulkanContext();
	virtual ~FVulkanContext();

	virtual void Init() override;
	virtual void Cleanup() override;
	virtual void Flush() override;

	virtual void BeginFrame(bool bClearBackBuffer) override;
	virtual void EndFrame() override;
	virtual void PresentFrame() override;

	virtual void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func);

//Command
	virtual void SetVSync(bool bEnable) override;
	virtual void SetViewport(uint32 InX, uint32 InY, uint32 InWidth, uint32 InHeight) override;
	virtual void SetClearColor(const Math::FLinearColor& InColor) override;
	virtual void SetClearDepthStencil(float InDepth, uint32 InStencil);
	virtual void ClearBackBuffer() override;

//Data transfer
	virtual void UpdateTexture(FRHITexture* InTexture, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;
	virtual void ClearTexture(FRHITexture* InTexture, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount) override;
	virtual void UpdateBufferData(const Ref<FRHIVertexBuffer>& Buffer, const void* Content, uint32 Size, uint32 Offset) override;
	virtual void UpdateBufferData(const Ref<FRHIIndexBuffer>& Buffer, const void* Content, uint32 Size, uint32 Offset) override;

//Draw
	virtual void BeginRenderPass(FRHIRenderPass* InRenderPass) override;
	virtual void EndRenderPass(bool bBlitToBack) override;
	virtual void NextSubpass() override;

	virtual void BindUniformBuffer(uint16 InBinding, FRHIUniformBuffer* InBuffer) override;
	virtual void BindTexture(uint16 InBinding, FRHITexture* InTexture, FRHISamplerState* InSamplerState) override;
	virtual void BindProgram(FRHIShaderProgram* InProgram) override;

	virtual void SetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState) override;

	virtual void DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive) override;

	virtual void RHISetVertexStream(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset) override;
	virtual void RHIDrawPrimitive(uint32 NumPrimitives, uint32 StartVertex) override;
	virtual void RHIDrawPrimitiveIndexed(FRHIBuffer* IndexBuffer, uint32 NumPrimitives, uint32 StartIndex, int32 VertexOffset) override;

	const FVulkanSwapchain& GetSwapchain() const { return mSwapchain; }
	VkImage GetSwapchainImage() { return mSwapchain.GetImages()[mCurSwapchainImageIndex]; }
	VkImageView GetSwapchainImageView() { return mSwapchain.GetImageViews()[mCurSwapchainImageIndex]; }

	FVulkanFrameData& GetActiveFrameData() { return mFrameData[mFrameDataIndex]; }
	VkCommandBuffer GetActiveCmdBuffer() { return mFrameData[mFrameDataIndex].CmdBuffer; }
	FVulkanDescriptorPool& GetActiveDescriptorPool() { return mFrameData[mFrameDataIndex].DescriptorPool; }

public:
	static FVulkanContext* Cast(FRHIContext* InContext);

private:
	void CreateImmediateData();
	VkDescriptorSet GetDescriptorSet(const FVulkanShaderProgram* InProgram, const FVulkanPipelineLayout* InLayout);

private:
	FVulkanSwapchain mSwapchain;
	VkPresentModeKHR mCurPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	VkPresentModeKHR mTargetPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

	//frame data
	uint32 mCurSwapchainImageIndex = 0;
	uint32 mFrameDataIndex = 0;
	FVulkanFrameData mFrameData[REV_VK_FRAME_OVERLAP];

	//dynamic state
	VkClearColorValue mClearColor;
	VkClearDepthStencilValue mClearDepthStencil;
	VkExtent2D mDrawExtent = {0, 0};
	VkViewport mViewport;
	VkRect2D mScissor;
	

	//immediate draw
	VkFence mImmFence;
	VkCommandBuffer mImmCmdBuffer;
	VkCommandPool mImmCmdPool;

	FVulkanGraphicsPipelineCache mGraphicsPipelineCache;
	FVulkanGraphicsFrameState mFrameState;

};


}