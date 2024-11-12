#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Math/Maths.h"
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

class FVulkanContext : public IRHIContext
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
	virtual void RHISetVSync(bool bEnable) override;
	virtual void RHISetViewport(uint32 InX, uint32 InY, uint32 InWidth, uint32 InHeight) override;
	virtual void RHIClearBackTexture(const Math::FLinearColor& InColor) override;

//Data transfer
	virtual void RHIUpdateTexture(FRHITexture* InTexture, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) override;
	virtual void RHIClearTexture(FRHITexture* InTexture, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount) override;
	virtual void RHIBlitTexture(FRHITexture* DstTexture, FRHITexture* SrcTexture) override;
	virtual void RHIBlitToBackTexture(FRHITexture* SrcTexture) override;


	virtual void RHIUpdateBufferData(FRHIBuffer* Buffer, const void* Content, uint32 Size, uint32 Offset = 0) override;

//Draw
	virtual void RHIBeginRenderPass(FRHIRenderPass* InRenderPass) override;
	virtual void RHIEndRenderPass() override;
	virtual void RHINextSubpass() override;

	virtual void RHIBindUniformBuffer(uint16 InBinding, FRHIUniformBuffer* InBuffer) override;
	virtual void RHIBindTexture(uint16 InBinding, FRHITexture* InTexture, FRHISamplerState* InSamplerState) override;
	virtual void RHIBindProgram(FRHIShaderProgram* InProgram) override;

	virtual void RHISetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState) override;

	virtual void RHISetVertexStream(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset) override;
	virtual void RHIDrawPrimitive(uint32 NumPrimitives, uint32 StartVertex) override;
	virtual void RHIDrawPrimitiveIndexed(FRHIBuffer* IndexBuffer, uint32 NumPrimitives, uint32 StartIndex, int32 VertexOffset) override;

	virtual void RHIBeginDebugLabel(const char* LabelContext, const Math::FLinearColor& Color) override;
	virtual void RHIEndDebugLabel() override;

	const FVulkanSwapchain& GetSwapchain() const { return mSwapchain; }
	VkImage GetSwapchainImage() const { return mSwapchain.GetCurrentImage(); }
	VkImageView GetSwapchainImageView() const { return mSwapchain.GetCurrentImageView(); }
	FVulkanTextureSwapchain* GetSwapchainTexture() const { return mSwapchain.GetCurrentTexture(); }

	FVulkanFrameData& GetActiveFrameData() { return mFrameData[mFrameDataIndex]; }
	VkCommandBuffer GetActiveCmdBuffer() { return mFrameData[mFrameDataIndex].CmdBuffer; }
	FVulkanDescriptorPool& GetActiveDescriptorPool() { return mFrameData[mFrameDataIndex].DescriptorPool; }

public:
	static FVulkanContext* Cast(IRHIContext* InContext);

private:
	void CreateImmediateData();
	VkDescriptorSet GetDescriptorSet(const FVulkanShaderProgram* InProgram, const FVulkanPipelineLayout* InLayout);

private:
	FVulkanSwapchain mSwapchain;
	VkPresentModeKHR mCurPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	VkPresentModeKHR mTargetPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

	//frame data
	uint32 mFrameDataIndex = 0;
	FVulkanFrameData mFrameData[REV_VK_FRAME_OVERLAP];

	//dynamic state
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