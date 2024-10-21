#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHIPrimitive.h"
#include "Rev/Render/RHI/DynamicRHI.h"

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
		const FTextureDesc TextureDesc = FTextureDesc::Make2D(1, 1, PF_R8G8B8A8).SetClearColor(ClearColor);
		TextureRHI = GDynamicRHI->CreateTexture(TextureDesc);

		const FSamplerStateDesc SamplerDesc(SF_Nearest, SW_Repeat, SW_Repeat, SW_Repeat);
		SamplerStateRHI = GDynamicRHI->CreateSamplerState(SamplerDesc);
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


namespace
{
static Ref<FRHIPrimitive> CreateScreenQuad()
{
    constexpr float ScreenQuadVertices[] = {
        -1.0, -1.0, 0.0,
        -1.0, +1.0, 0.0,
        +1.0, +1.0, 0.0,
        +1.0, -1.0, 0.0,
    };

    constexpr uint16 ScreenQuadIndices[] = {
        0, 3, 1,
        2, 1, 3
    };

    constexpr uint32 VertexSize = sizeof(ScreenQuadVertices);
    Ref<FRHIVertexBuffer> VertexBuffer = GDynamicRHI->CreateVertexBuffer(VertexSize);
    FRenderCore::GetMainContext()->UpdateBufferData(VertexBuffer, ScreenQuadVertices, VertexSize);
    VertexBuffer->SetLayout({
        {"Position", VT_Float3, 0}
        });

    constexpr uint32 IndexCount = sizeof(ScreenQuadIndices) / sizeof(uint16);
    Ref<FRHIIndexBuffer> IndexBuffer = GDynamicRHI->CreateIndexBuffer(sizeof(uint16), IndexCount);
    FRenderCore::GetMainContext()->UpdateBufferData(IndexBuffer, ScreenQuadIndices, sizeof(ScreenQuadIndices));

    Ref<FRHIPrimitive> Primitive = GDynamicRHI->CreatePrimitive(PT_Triangles);
    Primitive->AddVertexBuffer(VertexBuffer);
    Primitive->SetIndexBuffer(IndexBuffer);
    return Primitive;
}
}

Ref<FRHIPrimitive> GScreenPlanePrimitive = nullptr;

void RenderUtils::Init()
{
    InitGlobalResources();

    GScreenPlanePrimitive = CreateScreenQuad();
}

void RenderUtils::Shutdown()
{
    ReleaseGlobalResources();

    SAFE_DELETE(GWhiteTexture);
    SAFE_DELETE(GBlackTexture);
    SAFE_DELETE(GNormalTexture);

    GScreenPlanePrimitive.reset();
}


}