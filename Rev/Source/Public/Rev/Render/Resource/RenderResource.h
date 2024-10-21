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



class FRHITexture;
class FRHISamplerState;
class FTexture : public FRenderResource
{ 
public:

	Ref<FRHITexture> TextureRHI;
	Ref<FRHISamplerState> SamplerStateRHI;
	bool bSRGB;

	FTexture()
		: TextureRHI(nullptr)
		, SamplerStateRHI(nullptr)
		, bSRGB(false)
	{}

	virtual ~FTexture() {};

	virtual uint32 GetSizeX() const;
	virtual uint32 GetSizeY() const;
	virtual uint32 GetSizeZ() const;

	FRHITexture* GetTextureRHI() const { return TextureRHI.get(); }
	FRHISamplerState* GetSamplerStateRHI() const { return SamplerStateRHI.get(); }

	virtual void ReleaseRHI() override
	{
		TextureRHI.reset();
		SamplerStateRHI.reset();
	}
};


}