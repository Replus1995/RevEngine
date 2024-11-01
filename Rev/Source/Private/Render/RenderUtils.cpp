#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHIState.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/Resource/TextureResource.h"

namespace Rev
{

template <int32 R, int32 G, int32 B, int32 A, bool bSRGB = true>
class FColoredTexture : public FTexture
{
public:
	// FResource interface.
	virtual void InitRHI() override
	{
		const FRHITextureClearColor ClearColor(Math::FLinearColor::FromColor(Math::FColor(R, G, B, A), bSRGB));
		FRHITextureDesc TextureDesc = FRHITextureDesc::Make2D(1, 1, PF_R8G8B8A8).SetClearColor(ClearColor).SetFlags(ETextureCreateFlags::ShaderResource);
		if(bSRGB)
			TextureDesc.AddFlags(ETextureCreateFlags::SRGB);

		TextureRHI = GDynamicRHI->RHICreateTexture(TextureDesc);

		const FRHISamplerStateDesc SamplerDesc(SF_Nearest, SW_Repeat, SW_Repeat, SW_Repeat);
		SamplerStateRHI = GDynamicRHI->RHICreateSamplerState(SamplerDesc);

		FRenderCore::GetMainContext()->RHIClearTexture(TextureRHI.get());
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
FTexture* GNormalTexture = new TGlobalResource<FColoredTexture<127, 127, 255, 255, false>>;

TGlobalResource<F2DQuadIndexBuffer> G2DQuadIndexBuffer;
TGlobalResource<F2DQuadVertexBuffer> G2DQuadVertexBuffer;
TGlobalResource<FTileVertexInputState> GTileVertexInputState;
TGlobalResource<FNullVertexBuffer> GNullVertexBuffer;
TGlobalResource<FStaticMeshVertexInputState> GStaticMeshVertexInputState;
TGlobalResource<FDefaultSamplerState> GDefaultSamplerState;


void FRenderUtils::Init()
{
    InitGlobalResources();
}

void FRenderUtils::Shutdown()
{
    ReleaseGlobalResources();

    SAFE_DELETE(GWhiteTexture);
    SAFE_DELETE(GBlackTexture);
    SAFE_DELETE(GNormalTexture);
}

void FRenderUtils::PostProcessDraw(FRHICommandList& RHICmdList)
{

	RHICmdList.GetContext()->RHISetVertexStream(0, G2DQuadVertexBuffer.VertexBufferRHI.get());
	RHICmdList.GetContext()->RHIDrawPrimitiveIndexed(G2DQuadIndexBuffer.IndexBufferRHI.get(), 2, 0);
}


}