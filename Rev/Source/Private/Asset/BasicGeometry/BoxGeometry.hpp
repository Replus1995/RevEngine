#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHICore.h"

namespace Rev
{


static constexpr float sBoxVertices[] = {
    -0.5, -0.5, -0.5,
    -0.5, +0.5, -0.5,
    +0.5, +0.5, -0.5,
    +0.5, -0.5, -0.5,
    -0.5, -0.5, +0.5,
    -0.5, +0.5, +0.5,
    +0.5, +0.5, +0.5,
    +0.5, -0.5, +0.5
};

static constexpr uint32 sBoxIndices[] = {
    4, 7, 5,
    6, 5, 7,
    7, 4, 3,
    0, 3, 4,
    6, 7, 2,
    3, 2, 7,
    5, 6, 1,
    2, 1, 6,
    4, 5, 0,
    1, 0, 5,
    0, 1, 3,
    2, 3, 1
};

class FBoxGeometry
{
public:
    static Ref<StaticMesh> Create(const Ref<SurfaceMaterial>& InMat)
    {
        std::vector<Ref<SurfaceMaterial>> boxMatArr = { InMat };
        std::vector<FMeshPrimitive> boxPrimArr;
        {
            constexpr uint32 boxVerticesSize = sizeof(sBoxVertices);
            Ref<FRHIVertexBuffer> boxVertices = FRHICore::CreateVertexBuffer(sBoxVertices, boxVerticesSize);
            boxVertices->SetLayout({
                {"Position", EVertexElementType::Float3,  0}
                //{EShaderDataType::Float3, "a_Normal"},
                //{EShaderDataType::Float2, "a_TexCoord"}
                });

            constexpr uint32 boxIndicesCount = sizeof(sBoxIndices) / sizeof(uint32);
            Ref<FRHIIndexBuffer> boxIndices = FRHICore::CreateIndexBuffer(sBoxIndices, EIndexElementType::UInt32, boxIndicesCount);

            FMeshPrimitive boxMeshPrim;
            boxMeshPrim.PrimitiveData = FRHICore::CreatePrimitive(PT_Triangles);
            boxMeshPrim.PrimitiveData->AddVertexBuffer(boxVertices);
            boxMeshPrim.PrimitiveData->SetIndexBuffer(boxIndices);

            boxPrimArr.emplace_back(std::move(boxMeshPrim));
        }

        Ref<StaticMesh> OutMesh = CreateRef<StaticMesh>();
        OutMesh->SetMaterials(std::move(boxMatArr));
        OutMesh->SetPrimitives(std::move(boxPrimArr));
        return OutMesh;
    }
};

}