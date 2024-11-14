#pragma once
#include "Rev/Render/Resource/RenderResource.h"
#include "Rev/Core/Buffer.h"

namespace Rev
{

class FRHITexture;
class FRHISamplerState;

class REV_API FTexture : public FRenderResource
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


//struct FTextureSource
//{
//	std::string Name;
//	FRHISamplerStateDesc SamplerDesc;
//	FRHITextureDesc TextureDesc;
//	std::vector<FBuffer> TextureSlices;
//
//	FTextureSource() = default;
//	~FTextureSource() = default;
//	FTextureSource(const FTextureSource&) = delete;
//
//	REV_API  Ref<FTexture> CreateFromSource();
//};

}