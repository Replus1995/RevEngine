#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RHI/RHIContext.h"

namespace Rev
{

FRHICommandList::FRHICommandList(IRHIContext* InContext)
	: mContext(InContext)
{
	REV_CORE_ASSERT(mContext);
}

FRHICommandList::~FRHICommandList()
{
}

void FRHICommandList::Transition(std::span<const FRHITextureBarrier> InTextureBarriers, std::span<const FRHIBufferBarrier> InBufferBarriers)
{
	mContext->RHITransition(InTextureBarriers, InBufferBarriers);
}

void FRHICommandList::BeginRendering(const FRHIRenderingInfo& InInfo)
{
	mContext->RHIBeginRendering(InInfo);
}

void FRHICommandList::EndRendering()
{
	mContext->RHIEndRendering();
}

void FRHICommandList::SetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState, const FRHIRenderTargetLayout& InLayout)
{
	mContext->RHISetGraphicsPipelineState(InState);
	mContext->RHISetRenderTargetLayout(InLayout);
}

void FRHICommandList::SetRenderTargetLayout(const FRHIRenderTargetLayout& InLayout)
{
	mContext->RHISetRenderTargetLayout(InLayout);
}

void FRHICommandList::SetViewport(uint32 X, uint32 Y, uint32 Width, uint32 Height) { mContext->RHISetViewport(X, Y, Width, Height); }
uint32 FRHICommandList::GetFrameWidth() const { return mContext->RHIGetFrameWidth(); }
uint32 FRHICommandList::GetFrameHeight() const { return mContext->RHIGetFrameHeight(); }
FRHITexture* FRHICommandList::GetBackTexture() const { return mContext->RHIGetBackTexture(); }
void FRHICommandList::BindUniformBuffer(uint16 Binding, FRHIUniformBuffer* Buffer) { mContext->RHIBindUniformBuffer(Binding, Buffer); }
void FRHICommandList::BindTexture(uint16 Binding, FRHITexture* Texture, FRHISamplerState* Sampler) { mContext->RHIBindTexture(Binding, Texture, Sampler); }
void FRHICommandList::BindProgram(FRHIShaderProgram* Program) { mContext->RHIBindProgram(Program); }
void FRHICommandList::SetGraphicsPipelineState(const FRHIGraphicsPipelineStateDesc& InState) { mContext->RHISetGraphicsPipelineState(InState); }
void FRHICommandList::SetVertexStream(uint32 StreamIndex, FRHIBuffer* Buffer, uint32 Offset) { mContext->RHISetVertexStream(StreamIndex, Buffer, Offset); }
void FRHICommandList::DrawPrimitive(uint32 NumPrimitives, uint32 StartVertex) { mContext->RHIDrawPrimitive(NumPrimitives, StartVertex); }
void FRHICommandList::DrawPrimitiveIndexed(FRHIBuffer* Buffer, uint32 NumPrimitives, uint32 StartIndex, int32 VertexOffset) { mContext->RHIDrawPrimitiveIndexed(Buffer, NumPrimitives, StartIndex, VertexOffset); }
void FRHICommandList::BeginDebugLabel(const char* Text, const Math::FLinearColor& Color) { mContext->RHIBeginDebugLabel(Text, Color); }
void FRHICommandList::EndDebugLabel() { mContext->RHIEndDebugLabel(); }

}


