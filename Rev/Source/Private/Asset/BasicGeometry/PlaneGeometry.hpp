#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

namespace Rev
{

static constexpr float sPlaneVertices[] = {
    -0.5, -0.5, 0.0,
    -0.5, 0.5, 0.0,
    0.5, 0.5, 0.0,
    0.5, -0.5, 0.0
};

static constexpr uint32 sPlaneIndices[] = {
    0, 1, 3,
    2, 3, 1
};

class FPlaneGeometry
{
public:
    static Ref<StaticMesh> Create(const Ref<Material>& InMat)
    {
        std::vector<Ref<Material>> planeMatArr = { InMat };
        std::vector<MeshPrimitive> planePrimArr;
        {
            constexpr uint32 planeVerticesSize = sizeof(sPlaneVertices);
            Ref<FRHIVertexBuffer> planeVertices = FRHIResourceFactory::CreateVertexBuffer(sPlaneVertices, planeVerticesSize);
            planeVertices->SetLayout({
                {"Position", EVertexElementType::Float3,  0}
                //{EShaderDataType::Float3, "a_Normal"},
                //{EShaderDataType::Float2, "a_TexCoord"}
                });

            constexpr uint32 planeIndicesCount = sizeof(sPlaneIndices) / sizeof(uint32);
            Ref<FRHIIndexBuffer> planeIndices = FRHIResourceFactory::CreateIndexBuffer(sPlaneIndices, sizeof(uint32), planeIndicesCount);

            MeshPrimitive planeMeshPrim;
            planeMeshPrim.VertexData = FRHIResourceFactory::CreateVertexArray();
            planeMeshPrim.VertexData->AddVertexBuffer(planeVertices);
            planeMeshPrim.VertexData->SetIndexBuffer(planeIndices);

            planePrimArr.emplace_back(std::move(planeMeshPrim));
        }

        Ref<StaticMesh> OutMesh = CreateRef<StaticMesh>();
        OutMesh->SetMaterials(std::move(planeMatArr));
        OutMesh->SetPrimitives(std::move(planePrimArr));
        return OutMesh;
    }
};

}