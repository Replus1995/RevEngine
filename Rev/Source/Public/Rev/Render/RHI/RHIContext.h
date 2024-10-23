#pragma once
#include <memory>
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
class FRHIVertexBuffer;
class FRHIIndexBuffer;
class FRHIBuffer;
class FRHIUniformBuffer;
class FRHIPrimitive;
class FRHIShaderProgram;
class FRHITexture;
class FRHISamplerState;
class FRHIRenderTarget;
class FRHIRenderPass;
class FRHIGraphicsPipelineStateDesc;

class FRHIContext
{
public:
	virtual ~FRHIContext() = default;

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual void Flush() = 0;

	virtual void BeginFrame(bool bClearBackBuffer) = 0;
	virtual void EndFrame() = 0;
	virtual void PresentFrame() = 0;

	//virtual void BeginCompute() = 0;
	//virtual void EndCompute() = 0;

	virtual void SetVSync(bool bEnable) = 0;
	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;
	virtual void SetClearColor(const Math::FLinearColor& color) = 0;
	virtual void ClearBackBuffer() = 0; //to be removed

	virtual void BeginRenderPass(FRHIRenderPass* InRenderPass) = 0;
	virtual void EndRenderPass(bool bBlitToBack = false) = 0;
	virtual void NextSubpass() = 0;


	virtual void UpdateTexture(FRHITexture* InTexture, const void* InContent, uint32 InSize, uint8 InMipLevel = 0, uint16 InArrayIndex = 0) = 0;
	virtual void ClearTexture(FRHITexture* InTexture, uint8 InMipLevel = 0, uint8 InMipCount = 1, uint16 InArrayIndex = 0, uint16 InArrayCount = 1) = 0;
	/**
	* @brief Update index buffer data
	* @param Content : memory pointer
	* @param Size : content size
	* @param Offset : content offset
	*/
	virtual void UpdateBufferData(const Ref<FRHIVertexBuffer>& Buffer, const void* Content, uint32 Size, uint32 Offset = 0) = 0;
	virtual void UpdateBufferData(const Ref<FRHIIndexBuffer>& Buffer, const void* Content, uint32 Size, uint32 Offset = 0) = 0;
	virtual void UpdateBufferData(FRHIBuffer* Buffer, const void* Content, uint32 Size, uint32 Offset = 0) = 0;

	//RHILockBuffer
	//RHIUnlockBuffer

	virtual void BindUniformBuffer(uint16 InBinding, FRHIUniformBuffer* InBuffer) = 0;
	virtual void BindTexture(uint16 InBinding, FRHITexture* InTexture, FRHISamplerState* InSamplerState = nullptr) = 0;
	virtual void BindProgram(FRHIShaderProgram* InProgram) = 0;

	virtual void SetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState) = 0;

	virtual void DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive) = 0;


	virtual void RHISetVertexStream(uint32 StreamIndex, FRHIBuffer* VertexBuffer,  uint32 Offset = 0) = 0;
	virtual void RHIDrawPrimitive(uint32 NumPrimitives, uint32 StartVertex = 0) = 0;
	virtual void RHIDrawPrimitiveIndexed(FRHIBuffer* IndexBuffer, uint32 NumPrimitives, uint32 StartIndex = 0, int32 VertexOffset = 0) = 0;

};
}