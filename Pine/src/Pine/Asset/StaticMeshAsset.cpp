#include "pinepch.h"
#include "StaticMeshAsset.h"
#include "Pine/Render/Resource/VertexBuffer.h"

namespace Pine
{

static constexpr float sSimpleBoxPositons[] = {
    -0.5, -0.5, -0.5,
    -0.5, 0.5, -0.5,
    0.5, 0.5, -0.5,
    0.5, -0.5, -0.5,
    -0.5, -0.5, 0.5,
    -0.5, 0.5, 0.5,
    0.5, 0.5, 0.5,
    0.5, -0.5, 0.5
};

static constexpr uint32 sSimpleBoxIndices[] = {
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

void StaticMeshAsset::SetDefaultMaterial(const Ref<Material>& mat)
{
    GetDefaultMaterial() = mat;
}

Ref<StaticMesh> StaticMeshAsset::CreateBox()
{
    std::vector<Ref<Material>> boxMatArr = { GetDefaultMaterial() };
    std::vector<StaticMeshResource> boxResArr;
    {
        constexpr uint32 boxVerticesSize = sizeof(sSimpleBoxPositons);
        Ref<VertexBuffer> boxVertices= VertexBuffer::Create(boxVerticesSize);
        boxVertices->SetLayout({
            {EShaderDataType::Float3, "a_Position"},
            //{EShaderDataType::Float3, "a_Normal"},
            //{EShaderDataType::Float2, "a_TexCoord"}
         });
        boxVertices->SetData(sSimpleBoxPositons, boxVerticesSize);

        constexpr uint32 boxIndicesCount = sizeof(sSimpleBoxIndices) / sizeof(uint32);
        Ref<IndexBuffer> boxIndices =  IndexBuffer::Create(sSimpleBoxIndices, boxIndicesCount);

        StaticMeshResource boxMeshRes;
        boxMeshRes.VertexData = VertexArray::Create();
        boxMeshRes.VertexData->AddVertexBuffer(boxVertices);
        boxMeshRes.VertexData->SetIndexBuffer(boxIndices);

        boxResArr.emplace_back(std::move(boxMeshRes));
    }

    return CreateRef<StaticMesh>(boxMatArr, boxResArr);
}

Ref<Material>& Pine::StaticMeshAsset::GetDefaultMaterial()
{
    static Ref<Material> sStaticMeshDefaultMaterial = nullptr;
    return sStaticMeshDefaultMaterial;
}

}
