#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include <span>

namespace Rev
{
class IRHIContext;
class FRHIGraphicsPipelineStateDesc;
class FRHIBuffer;
class FRHIUniformBuffer;
class FRHIShaderProgram;
class FRHITexture;
class FRHISamplerState;
class FRHICommandList
{
public:
	FRHICommandList(IRHIContext* InContext);
	~FRHICommandList();

	IRHIContext* GetContext() { return mContext; };
	void Transition(std::span<const FRHITextureBarrier> InTextureBarriers, std::span<const FRHIBufferBarrier> InBufferBarriers = {});
	void BeginRendering(const FRHIRenderingInfo& InInfo);
	void EndRendering();
	void SetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState, const FRHIRenderTargetLayout& InLayout);
	void SetRenderTargetLayout(const FRHIRenderTargetLayout& InLayout);
	void SetViewport(uint32 X, uint32 Y, uint32 Width, uint32 Height);
	uint32 GetFrameWidth() const;
	uint32 GetFrameHeight() const;
	FRHITexture* GetBackTexture() const;
	void BindUniformBuffer(uint16 Binding, FRHIUniformBuffer* Buffer);
	void BindTexture(uint16 Binding, FRHITexture* Texture, FRHISamplerState* Sampler = nullptr);
	void BindProgram(FRHIShaderProgram* Program);
	void SetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState);
	void SetVertexStream(uint32 StreamIndex, FRHIBuffer* Buffer, uint32 Offset = 0);
	void DrawPrimitive(uint32 NumPrimitives, uint32 StartVertex = 0);
	void DrawPrimitiveIndexed(FRHIBuffer* IndexBuffer, uint32 NumPrimitives, uint32 StartIndex = 0, int32 VertexOffset = 0);
	void BeginDebugLabel(const char* Text, const Math::FLinearColor& Color);
	void EndDebugLabel();

protected:
	IRHIContext* mContext;
};

}
