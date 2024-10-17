#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/SurfaceMaterial.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"

namespace Rev
{

static constexpr float sPlaneVertices[] = {
    -0.5, -0.5, 0.0, 0.0, 0.0,
    -0.5, +0.5, 0.0, 0.0, 1.0,
    +0.5, +0.5, 0.0, 1.0, 1.0,
    +0.5, -0.5, 0.0, 1.0, 0.0
};

static constexpr uint32 sPlaneIndices[] = {
    0, 3, 1,
    2, 1, 3
};

class FPlaneGeometry
{
public:
    static Ref<StaticMesh> Create(const Ref<SurfaceMaterial>& InMat)
    {
        std::vector<Ref<SurfaceMaterial>> planeMatArr = { InMat };
        std::vector<FMeshPrimitive> planePrimArr;
        {
            FMeshPrimitive planeMeshPrim;
            planeMeshPrim.PrimitiveData = MakeVertexData();
            planePrimArr.emplace_back(std::move(planeMeshPrim));
        }

        Ref<StaticMesh> OutMesh = CreateRef<StaticMesh>();
        OutMesh->SetMaterials(std::move(planeMatArr));
        OutMesh->SetPrimitives(std::move(planePrimArr));
        return OutMesh;
    }

    static Ref<FRHIPrimitive> MakeVertexData()
    {
        constexpr uint32 planeVerticesSize = sizeof(sPlaneVertices);
        Ref<FRHIVertexBuffer> planeVertices = GDynamicRHI->CreateVertexBuffer(planeVerticesSize);
        FRenderCore::GetMainContext()->UpdateBufferData(planeVertices, sPlaneVertices, planeVerticesSize);
        planeVertices->SetLayout({
            {"Position", VT_Float3, 0},
            {"TexCoord", VT_Float2, 1}
            });

        constexpr uint32 planeIndicesCount = sizeof(sPlaneIndices) / sizeof(uint32);
        Ref<FRHIIndexBuffer> planeIndices = GDynamicRHI->CreateIndexBuffer(sizeof(uint32), planeIndicesCount);
        FRenderCore::GetMainContext()->UpdateBufferData(planeIndices, sPlaneIndices, sizeof(sPlaneIndices));

        Ref<FRHIPrimitive> Result = GDynamicRHI->CreatePrimitive(PT_Triangles);
        Result->AddVertexBuffer(planeVertices);
        Result->SetIndexBuffer(planeIndices);
        return Result;
    }
};

}