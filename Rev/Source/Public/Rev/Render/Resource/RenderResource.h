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
	Ref<FRHIRasterizerState> RasterizerStateRHI;

	FRasterizerState() {}
	virtual ~FRasterizerState() {}
	virtual void ReleaseRHI() override
	{
		RasterizerStateRHI.reset();
	}
};

class REV_API FDepthStencilState : public FRenderResource
{
public:
	Ref<FRHIDepthStencilState> DepthStencilStateRHI;

	FDepthStencilState() {}
	virtual ~FDepthStencilState() {}
	virtual void ReleaseRHI() override
	{
		DepthStencilStateRHI.reset();
	}
};

class REV_API FColorBlendState : public FRenderResource
{
public:
	Ref<FRHIColorBlendState> ColorBlendStateRHI;

	FColorBlendState() {}
	virtual ~FColorBlendState() {}
	virtual void ReleaseRHI() override
	{
		ColorBlendStateRHI.reset();
	}
};

class REV_API FVertexInputState : public FRenderResource
{
public:
	Ref<FRHIVertexInputState> VertexInputStateRHI;

	FVertexInputState() {}
	virtual ~FVertexInputState() {}
	virtual void ReleaseRHI() override
	{
		VertexInputStateRHI.reset();
	}
};


}