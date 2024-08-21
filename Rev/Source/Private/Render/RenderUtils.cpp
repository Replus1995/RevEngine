#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

namespace Rev
{

static Ref<FRHIVertexArray> sScreenQuad = nullptr;
static Ref<FRHIVertexArray> CreateScreenQuad()
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
    Ref<FRHIVertexBuffer> VertexBuffer = FRHIResourceFactory::CreateVertexBuffer(ScreenQuadVertices, VertexSize);
    VertexBuffer->SetLayout({
        {"Position", EVertexElementType::Float3, 0}
        });

    constexpr uint32 IndexCount = sizeof(ScreenQuadIndices) / sizeof(uint16);
    Ref<FRHIIndexBuffer> IndexBuffer = FRHIResourceFactory::CreateIndexBuffer(ScreenQuadIndices, sizeof(uint16), IndexCount);

    Ref<FRHIVertexArray> VertexArray = FRHIResourceFactory::CreateVertexArray();
    VertexArray->AddVertexBuffer(VertexBuffer);
    VertexArray->SetIndexBuffer(IndexBuffer);
    return VertexArray;
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
    RenderCmd::Draw(sScreenQuad, DM_Triangles);
}

}