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

template<typename ParameterStructType>
inline FRGPassRef FRGBuilder::AddPass(FRGName&& InName, const ParameterStructType* ParameterStruct, ERGPassFlags Flags, PrepareLambdaFunc<ParameterStructType>&& PrepareLambda, ExecuteLambdaFunc<ParameterStructType>&& ExecuteLambda)
{
	return FRGPassRef();
}

}


