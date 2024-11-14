#include "Rev/Render/RenderGraph/RenderGraphBuilder.h"

namespace Rev
{

FRGBuilder::FRGBuilder(const FRGName& InName)
	: BuilderName(InName)
{
}

FRGBuilder::~FRGBuilder()
{
}

FRGTextureRef FRGBuilder::RegisterExternalTexture(const Ref<FRHITexture>& ExternalTexture, ERGTextureFlags InFlags)
{
	return FRGTextureRef();
}

FRGBufferRef FRGBuilder::RegisterExternalBuffer(const Ref<FRHIBuffer>& ExternalBuffer, ERGBufferFlags InFlags)
{
	return FRGBufferRef();
}

FRGTextureRef FRGBuilder::CreateTexture(const FRGTextureDesc& InDesc, const char* InName, ERGTextureFlags InFlags)
{
	return FRGTextureRef();
}

FRGBufferRef FRGBuilder::CreateBuffer(const FRGBufferDesc& InDesc, const char* InName, ERGBufferFlags InFlags)
{
	return FRGBufferRef();
}



}


