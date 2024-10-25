#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/PixelFormat.h"

namespace Rev
{

class REV_API FRenderResource
{
public:
	FRenderResource();
	virtual ~FRenderResource();

	virtual void InitRHI() {}
	virtual void ReleaseRHI() {}

	virtual void InitResource();
	virtual void ReleaseResource();
	void ReinitRHI();


	bool IsInitialized() const { return ListIndex != -1; }
	int32 GetListIndex() const { return ListIndex; }

private:
	int32 ListIndex;

public:
	//ERenderResourceState ResourceState = ERenderResourceState::Default;
};

template<class ResourceType>
class TGlobalResource : public ResourceType
{
public:
	TGlobalResource()
	{
		((ResourceType*)this)->InitResource();
	}

	template<typename... TParam>
	explicit TGlobalResource(TParam... Params)
		: ResourceType(std::forward<TParam...>(Params))
	{
		((ResourceType*)this)->InitResource();
	}

	virtual ~TGlobalResource()
	{
		((ResourceType*)this)->ReleaseResource();
	}
};

extern REV_API void InitGlobalResources();
extern REV_API void ReleaseGlobalResources();


class FRHIBuffer;
class FRHIRasterizerState;
class FRHIDepthStencilState;
class FRHIColorBlendState;
class FRHIVertexInputState;


class REV_API FVertexBuffer : public FRenderResource
{
public:
	Ref<FRHIBuffer> VertexBufferRHI;

	FVertexBuffer() {}
	virtual ~FVertexBuffer() {}
	virtual void ReleaseRHI() override
	{
		VertexBufferRHI.reset();
	}
};

class REV_API FIndexBuffer : public FRenderResource
{
public:
	Ref<FRHIBuffer> IndexBufferRHI;

	FIndexBuffer() {}
	virtual ~FIndexBuffer() {}
	virtual void ReleaseRHI() override
	{
		IndexBufferRHI.reset();
	}
};

class REV_API FRasterizerState : public FRenderResource
{
public:
	Ref<FRHIRasterizerState> SamplerStateRHI;

	FRasterizerState() {}
	virtual ~FRasterizerState() {}
	virtual void ReleaseRHI() override
	{
		SamplerStateRHI.reset();
	}
};


}