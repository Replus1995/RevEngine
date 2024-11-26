#pragma once
#include <memory>
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
class FRHIBuffer;
class FRHIUniformBuffer;
class FRHIShaderProgram;
class FRHITexture;
class FRHISamplerState;
class FRHIRenderTarget;
class FRHIRenderPass;
class FRHIGraphicsPipelineStateDesc;

class IRHIContext
{
public:
	virtual ~IRHIContext() = default;

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual void Flush() = 0;

	virtual void BeginFrame(bool bClearBackBuffer) = 0;
	virtual void EndFrame() = 0;
	virtual void PresentFrame() = 0;

	//virtual void BeginCompute() = 0;
	//virtual void EndCompute() = 0;

	virtual void RHISetVSync(bool bEnable) = 0;
	virtual void RHISetViewport(uint32 InX, uint32 InY, uint32 InWidth, uint32 InHeight) = 0;
	virtual void RHIClearBackTexture(const Math::FLinearColor& InColor) = 0;
	virtual uint32 RHIGetFrameWidth() = 0;
	virtual uint32 RHIGetFrameHeight() = 0;
	virtual FRHITexture* RHIGetBackTexture() = 0;

	virtual void RHIBeginRenderPass(FRHIRenderPass* InRenderPass) = 0;
	virtual void RHIEndRenderPass() = 0;
	virtual void RHINextSubpass() = 0;


	virtual void RHIUpdateTexture(FRHITexture* InTexture, const void* InContent, uint32 InSize, uint8 InMipLevel = 0, uint16 InArrayIndex = 0) = 0;
	virtual void RHIClearTexture(FRHITexture* InTexture, uint8 InMipLevel = 0, uint8 InMipCount = 1, uint16 InArrayIndex = 0, uint16 InArrayCount = 1) = 0;
	virtual void RHIBlitTexture(FRHITexture* DstTexture, FRHITexture* SrcTexture) = 0;
	virtual void RHIBlitToBackTexture(FRHITexture* SrcTexture) = 0;

	/**
	* @brief Update index buffer data
	* @param Content : memory pointer
	* @param Size : content size
	* @param Offset : content offset
	*/
	virtual void RHIUpdateBufferData(FRHIBuffer* Buffer, const void* Content, uint32 Size, uint32 Offset = 0) = 0;

	//RHILockBuffer
	//RHIUnlockBuffer

	virtual void RHIBindUniformBuffer(uint16 InBinding, FRHIUniformBuffer* InBuffer) = 0;
	virtual void RHIBindTexture(uint16 InBinding, FRHITexture* InTexture, FRHISamplerState* InSamplerState = nullptr) = 0;
	virtual void RHIBindTextures(uint16 InBinding, FRHITexture** InTextures, uint32 InNumTextures) = 0;
	virtual void RHIBindSamplerState(uint16 InBinding, FRHISamplerState* InSamplerState) = 0;

	virtual void RHIBindProgram(FRHIShaderProgram* InProgram) = 0;

	virtual void RHISetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState) = 0;

	virtual void RHISetVertexStream(uint32 StreamIndex, FRHIBuffer* VertexBuffer,  uint32 Offset = 0) = 0;
	virtual void RHIDrawPrimitive(uint32 NumPrimitives, uint32 StartVertex = 0) = 0;
	virtual void RHIDrawPrimitiveIndexed(FRHIBuffer* IndexBuffer, uint32 NumPrimitives, uint32 StartIndex = 0, int32 VertexOffset = 0) = 0;

//Debug
	virtual void RHIBeginDebugLabel(const char* LabelContext, const Math::FLinearColor& Color) = 0;
	virtual void RHIEndDebugLabel() = 0;

};
}