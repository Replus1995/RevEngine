#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/Resource/TextureResource.h"

namespace Rev
{

template <int32 R, int32 G, int32 B, int32 A>
class FColoredTexture : public FTexture
{
public:
	// FResource interface.
	virtual void InitRHI() override
	{
		const FRHITextureClearColor ClearColor(Math::FLinearColor::FromColor(Math::FColor(R, G, B, A)));
		const FRHITextureDesc TextureDesc = FRHITextureDesc::Make2D(1, 1, PF_R8G8B8A8).SetClearColor(ClearColor).SetFlags(ETextureCreateFlags::ShaderResource);
		TextureRHI = GDynamicRHI->RHICreateTexture(TextureDesc);

		const FRHISamplerStateDesc SamplerDesc(SF_Nearest, SW_Repeat, SW_Repeat, SW_Repeat);
		SamplerStateRHI = GDynamicRHI->RHICreateSamplerState(SamplerDesc);
	}
};

//template <float X, float Y, float Z>
//class FLinearTexture : public FTexture
//{
//public:
//    // FResource interface.
//    virtual void InitRHI() override
//    {
//        const FRHITextureClearColor ClearColor(Math::FLinearColor(X, Y, Z));
//        const FTextureDesc TextureDesc = FTextureDesc::Make2D(1, 1, PF_R8G8B8).SetClearColor(ClearColor);
//        TextureRHI = GDynamicRHI->CreateTexture(TextureDesc);
//
//        const FSamplerStateDesc SamplerDesc(SF_Nearest, SW_Repeat, SW_Repeat, SW_Repeat);
//        SamplerStateRHI = GDynamicRHI->CreateSamplerState(SamplerDesc);
//    }
//};


FTexture* GWhiteTexture = new TGlobalResource<FColoredTexture<255, 255, 255, 255>>;
FTexture* GBlackTexture = new TGlobalResource<FColoredTexture<0, 0, 0, 255>>;
FTexture* GNormalTexture = new TGlobalResource<FColoredTexture<127, 127, 255, 255>>;

TGlobalResource<F2DQuadIndexBuffer> G2DQuadIndexBuffer;
TGlobalResource<F2DQuadVertexBuffer> G2DQuadVertexBuffer;
TGlobalResource<FTileVertexInputState> GTileVertexInputState;
TGlobalResource<FNullVertexBuffer> GNullVertexBuffer;
TGlobalResource<FStaticMeshVertexInputState> GStaticMeshVertexInputState;
TGlobalResource<FDefaultSamplerState> GDefaultSamplerState;


void RenderUtils::Init()
{
    InitGlobalResources();
}

void RenderUtils::Shutdown()
{
    ReleaseGlobalResources();

    SAFE_DELETE(GWhiteTexture);
    SAFE_DELETE(GBlackTexture);
    SAFE_DELETE(GNormalTexture);
}


}