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

	virtual void Prepare(FRGPassRef Pass) {}
	virtual void Execute(FRHICommandList& RHICmdList) {}

	void InitRHI(const FRGRenderTargetBindings* RenderTargets);

	// Functions for adding dependency
	// void WriteTo(const FRGResource*);
	// void ReadFrom(const FRGResource*);

protected:
	bool IsReadyForDraw() const;
	void BeginPass(FRHICommandList& RHICmdList);
	void EndPass(FRHICommandList& RHICmdList);

	const FRGName Name;
	const FRGParameterStruct ParameterStruct;
	const ERGPassFlags Flags;
	ERHIPipeline Pipeline;
	FRHIRenderPassRef RenderPassRHI;
};


template<typename ParameterStructType>
class TRGLambdaPass : public FRGPass
{
public:
	using PrepareLambdaType = std::function<void(FRGPassRef, const ParameterStructType*)>;
	using ExecuteLambdaType = std::function<void(FRHICommandList&, const ParameterStructType*)>;

	TRGLambdaPass(
		FRGName&& InName,
		const ParameterStructType* InParameterStruct,
		ERGPassFlags InFlags,
		PrepareLambdaType&& InPrepareLambda,
		ExecuteLambdaType&& InExecuteLambda)
		: FRGPass(std::move(InName), FRGParameterStruct(InParameterStruct), InFlags)
		, PrepareLambda(std::move(InPrepareLambda))
		, ExecuteLambda(std::move(InExecuteLambda))
	{

	}

	virtual void Prepare(FRGPassRef Pass) override
	{
		if (PrepareLambda)
		{
			PrepareLambda(this, GetParametersAs<ParameterStructType>());
		}
	}

	virtual void Execute(FRHICommandList& RHICmdList) override
	{
		if (ExecuteLambda && IsReadyForDraw())
		{
			BeginPass(RHICmdList);
			ExecuteLambda(RHICmdList, GetParametersAs<ParameterStructType>());
			EndPass(RHICmdList);
		}
	}

private:
	PrepareLambdaType PrepareLambda = nullptr;
	ExecuteLambdaType ExecuteLambda = nullptr;
};

}