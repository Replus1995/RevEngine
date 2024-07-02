#include "pinepch.h"
#include "Mesh.h"
#include "Pine/Render/StaticMesh.h"
#include "Pine/Render/Resource/VertexBuffer.h"

namespace Pine
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
    std::vector<StaticMeshResource> boxResArr;
    {
        constexpr uint32 boxVerticesSize = sizeof(SimpleGeo::sBoxVertices);
        Ref<VertexBuffer> boxVertices = VertexBuffer::Create(boxVerticesSize);
        boxVertices->SetLayout({
            {EShaderDataType::Float3, "a_Position"},
            //{EShaderDataType::Float3, "a_Normal"},
            //{EShaderDataType::Float2, "a_TexCoord"}
            });
        boxVertices->SetData(SimpleGeo::sBoxVertices, boxVerticesSize);

        constexpr uint32 boxIndicesCount = sizeof(SimpleGeo::sBoxIndices) / sizeof(uint32);
        Ref<IndexBuffer> boxIndices = IndexBuffer::Create(SimpleGeo::sBoxIndices, boxIndicesCount);

        StaticMeshResource boxMeshRes;
        boxMeshRes.VertexData = VertexArray::Create();
        boxMeshRes.VertexData->AddVertexBuffer(boxVertices);
        boxMeshRes.VertexData->SetIndexBuffer(boxIndices);

        boxResArr.emplace_back(std::move(boxMeshRes));
    }

    return CreateRef<StaticMesh>(boxMatArr, boxResArr);
}

Ref<Material>& MeshUtils::GetDefaultMaterial()
{
	static Ref<Material> sMeshDefaultMaterial = nullptr;
	return sMeshDefaultMaterial;
}

}
