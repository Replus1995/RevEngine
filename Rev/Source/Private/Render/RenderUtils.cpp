#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHIPrimitive.h"

namespace Rev
{

static Ref<FRHIPrimitive> sScreenQuad = nullptr;
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

void RenderUtils::Init()
{
    //sScreenQuad = CreateScreenQuad();
}

void RenderUtils::Shutdown()
{
    sScreenQuad.reset();
}


void RenderUtils::DrawScreenQuad()
{
    //RenderCmd::DrawPrimitive(sScreenQuad,)
}

}