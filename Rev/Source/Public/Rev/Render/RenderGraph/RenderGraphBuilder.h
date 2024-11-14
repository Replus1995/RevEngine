#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"
#include "Rev/Render/RenderGraph/RenderGraphResources.h"


namespace Rev
{



class REV_API FRGBuilder
{
public:
	FRGBuilder();
	FRGBuilder(const FRGBuilder&) = delete;
	~FRGBuilder();

	FRGTextureRef CreateTexture(const FRGTextureDesc& InDesc, const char* InName, ERGTextureFlags InFlags = ERGTextureFlags::None);
	FRGBufferRef CreateBuffer(const FRGBufferDesc& InDesc, const char* InName, ERGBufferFlags InFlags = ERGBufferFlags::None);


	template<typename ParameterStructType>
	using PrepareLambdaFunc = std::function<void(FRGPassRef, const ParameterStructType*)>;

	template<typename ParameterStructType>
	using ExecuteLambdaFunc = std::function<void(FRHICommandList&, const ParameterStructType*)>;

	template <typename ParameterStructType>
	FRGPassRef AddPass(FRGName&& InName, const ParameterStructType* ParameterStruct, ERGPassFlags Flags, PrepareLambdaFunc<ParameterStructType>&& PrepareLambda, ExecuteLambdaFunc<ParameterStructType>&& ExecuteLambda);

	
};


}