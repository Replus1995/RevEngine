#pragma once
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"

namespace Rev
{
class FRGBuilder;
class REV_API FRGPassBuilder
{
public:
	FRGTextureHandle ReadTexture(FRGTextureHandle, ERHIAccess = ERHIAccess::ShaderRead, FRHITextureSubresourceRange = {});
	FRGTextureHandle WriteTexture(FRGTextureHandle, ERHIAccess, FRHITextureSubresourceRange = {});
	FRGTextureHandle UseColorAttachment(uint32 Slot, FRGTextureHandle, ERenderTargetLoadAction, ERenderTargetStoreAction = RTS_Store);
	FRGTextureHandle UseDepthStencil(FRGTextureHandle, ERenderTargetLoadAction, ERenderTargetStoreAction, bool bReadOnly);
private:
	FRGPassBuilder(FRGBuilder& InGraph, uint32 InPassIndex) : Graph(InGraph), PassIndex(InPassIndex) {}
	FRGBuilder& Graph;
	uint32 PassIndex;
	friend class FRGBuilder;
};
}
