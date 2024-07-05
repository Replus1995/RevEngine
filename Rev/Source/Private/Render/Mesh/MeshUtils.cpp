#include "Rev/Render/Mesh/MeshUtils.h"
#include "Rev/Render/Mesh/StaticMesh.h"
#include "Rev/Render/Resource/VertexBuffer.h"

namespace Rev
{

namespace SimpleGeo
{

static constexpr float sBoxVertices[] = {
    -0.5, -0.5, -0.5,
    -0.5, 0.5, -0.5,
    0.5, 0.5, -0.5,
    0.5, -0.5, -0.5,
    -0.5, -0.5, 0.5,
    -0.5, 0.5, 0.5,
    0.5, 0.5, 0.5,
    0.5, -0.5, 0.5
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

}

void MeshUtils::SetDefaultMaterial(const Ref<Material>& mat)
{
    GetDefaultMaterial() = mat;
}

Ref<StaticMesh> MeshUtils::CreateBox()
{
    std::vector<Ref<Material>> boxMatArr = { GetDefaultMaterial() };
    std::vector<FMeshPrimitive> boxPrimArr;
    {
        constexpr uint32 boxVerticesSize = sizeof(SimpleGeo::sBoxVertices);
        Ref<FVertexBuffer> boxVertices = FVertexBuffer::Create(SimpleGeo::sBoxVertices, boxVerticesSize);
        boxVertices->SetLayout({
            {"Position", EVertexElementType::Float3,  0}
            //{EShaderDataType::Float3, "a_Normal"},
            //{EShaderDataType::Float2, "a_TexCoord"}
            });

        constexpr uint32 boxIndicesCount = sizeof(SimpleGeo::sBoxIndices) / sizeof(uint32);
        Ref<FIndexBuffer> boxIndices = FIndexBuffer::Create(SimpleGeo::sBoxIndices, sizeof(uint32), boxIndicesCount);

        FMeshPrimitive boxMeshPrim;
        boxMeshPrim.VertexData = FVertexArray::Create();
        boxMeshPrim.VertexData->AddVertexBuffer(boxVertices);
        boxMeshPrim.VertexData->SetIndexBuffer(boxIndices);

        boxPrimArr.emplace_back(std::move(boxMeshPrim));
    }

    Ref<StaticMesh> OutMesh = CreateRef<StaticMesh>();
    OutMesh->SetMaterials(std::move(boxMatArr));
    OutMesh->SetPrimitives(std::move(boxPrimArr));
    return OutMesh;
}

Ref<Material>& MeshUtils::GetDefaultMaterial()
{
    static Ref<Material> sMeshDefaultMaterial = nullptr;
    return sMeshDefaultMaterial;
}


}