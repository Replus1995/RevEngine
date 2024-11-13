#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"
#include "Rev/Render/RenderGraph/RenderGraphParameter.h"

namespace Rev
{
class REV_API FRGPass
{
public:
	FRGPass(FRGName&& InName, FRGParameterStruct InParameterStruct, ERGPassFlags InFlags);
	FRGPass(const FRGPass&) = delete;
	virtual ~FRGPass() = default;

	const char* GetName() const;

	FORCEINLINE ERGPassFlags GetFlags() const
	{
		return Flags;
	}

	FORCEINLINE ERHIPipeline GetPipeline() const
	{
		return Pipeline;
	}

	FORCEINLINE FRGParameterStruct GetParameters() const
	{
		return ParameterStruct;
	}

	template<typename ParameterType>
	const ParameterType* GetParametersAs() const
	{
		return ParameterStruct.GetContentAs<ParameterType>();
	}

	virtual void Execute(FRHICommandList& RHICmdList) {}



protected:
	const FRGName Name;
	const ERGPassFlags Flags;
	const ERHIPipeline Pipeline;
	const FRGParameterStruct ParameterStruct;
	FRHIRenderPassRef RenderPassRHI;
};

}