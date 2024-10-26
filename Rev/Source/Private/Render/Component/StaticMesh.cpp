#include "Rev/Render/Component/StaticMesh.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FStaticMesh::FStaticMesh()
{
}

FStaticMesh::~FStaticMesh()
{
	VertexBuffer.ReleaseResource();
	IndexBuffer.ReleaseResource();
}

bool FStaticMesh::IsEmpty() const
{
	return VertexBuffer.GetNumVertices() == 0 && IndexBuffer.GetNumIndices() == 0;
}

std::vector<const FStaticMeshSection*> FStaticMesh::GetSectionsForMaterial(uint32 InMaterialIndex) const
{
	std::vector<const FStaticMeshSection*> OutSections;
	for (auto& Section : Sections)
	{
		if(Section.MaterialIndex == InMaterialIndex)
			OutSections.push_back(&Section);
	}
	return OutSections;
}

FStaticMeshBuilder::FStaticMeshBuilder()
{
	StaticMesh = CreateRef<FStaticMesh>();
}

FStaticMeshBuilder::~FStaticMeshBuilder()
{
}

void FStaticMeshBuilder::Init(uint32 InNumVertices, uint32 InNumTexCoords, uint32 InNumIndices, bool bUse32Bit)
{
	if (InNumVertices != StaticMesh->VertexBuffer.GetNumVertices() || InNumTexCoords != StaticMesh->VertexBuffer.GetNumTexCoords())
	{
		StaticMesh->VertexBuffer.Cleanup();
		StaticMesh->VertexBuffer.Init(InNumVertices, InNumTexCoords);
	}
	if (InNumIndices != StaticMesh->IndexBuffer.GetNumIndices() || bUse32Bit != StaticMesh->IndexBuffer.Is32Bit())
	{
		StaticMesh->IndexBuffer.Cleanup();
		StaticMesh->IndexBuffer.Init(InNumIndices, bUse32Bit);
	}

}

void FStaticMeshBuilder::InitVertices(uint32 InNumVertices, uint32 InNumTexCoords, FBuffer&& InPositionData, FBuffer&& InNormalData, FBuffer&& InTangentData, FBuffer* InTexCoordData, FBuffer&& InColorData)
{
	REV_CORE_ASSERT(InNumVertices * sizeof(Math::FVector3) == InPositionData.Size());

	StaticMesh->VertexBuffer.Cleanup();
	StaticMesh->VertexBuffer.NumVertices = InNumVertices;
	StaticMesh->VertexBuffer.NumTexCoords = InNumTexCoords;
	StaticMesh->VertexBuffer.PositionData = std::move(InPositionData);
	if (!InNormalData.Empty())
	{
		REV_CORE_ASSERT(InNumVertices * sizeof(Math::FVector3) == InNormalData.Size());
		StaticMesh->VertexBuffer.NormalData = std::move(InNormalData);
	}
	if (!InTangentData.Empty())
	{
		REV_CORE_ASSERT(InNumVertices * sizeof(Math::FVector4) == InTangentData.Size());
		StaticMesh->VertexBuffer.TangentData = std::move(InTangentData);
	}
	if (InNumTexCoords > 0)
	{
		for (uint32 i = 0; i < InNumVertices; i++)
		{
			StaticMesh->VertexBuffer.TexCoordData[i] = std::move(InTexCoordData[i]);
		}
	}
	if (!InColorData.Empty())
	{
		REV_CORE_ASSERT(InNumVertices * sizeof(Math::FColor) == InColorData.Size());
		StaticMesh->VertexBuffer.ColorData = std::move(InColorData);
	}
}

void FStaticMeshBuilder::InitIndices(uint32 InNumIndices, bool bUse32Bit, FBuffer&& InIndexData)
{
	uint32 IndexStride = bUse32Bit ? 4 : 2;
	REV_CORE_ASSERT(InNumIndices * IndexStride == InIndexData.Size());
	StaticMesh->IndexBuffer.Cleanup();
	StaticMesh->IndexBuffer.NumIndices = InNumIndices;
	StaticMesh->IndexBuffer.b32Bit = bUse32Bit;
	StaticMesh->IndexBuffer.IndexData = std::move(InIndexData);
}

void FStaticMeshBuilder::SetMaterials(const std::vector<Ref<FMaterial>>& InMaterials)
{
	StaticMesh->Materials = InMaterials;
}

void FStaticMeshBuilder::FillPositions(const float* Content, uint32 Size, uint32 Offset)
{
	StaticMesh->VertexBuffer.FillPositions(Content, Size, Offset);
}

void FStaticMeshBuilder::FillNormals(const float* Content, uint32 Size, uint32 Offset)
{
	StaticMesh->VertexBuffer.FillNormals(Content, Size, Offset);
}

void FStaticMeshBuilder::FillTangents(const float* Content, uint32 Size, uint32 Offset)
{
	StaticMesh->VertexBuffer.FillTangents(Content, Size, Offset);
}

void FStaticMeshBuilder::FillTexCoords(uint32 UVIndex, const float* Content, uint32 Size, uint32 Offset)
{
	StaticMesh->VertexBuffer.FillTexCoords(UVIndex, Content, Size, Offset);
}

void FStaticMeshBuilder::FillColors(const uint8* Content, uint32 Size, uint32 Offset)
{
	StaticMesh->VertexBuffer.FillColors(Content, Size, Offset);
}

void FStaticMeshBuilder::FillIndices(const uint16* Content, uint32 Size, uint32 Offset)
{
	StaticMesh->IndexBuffer.FillIndices(Content, Size, Offset);
}

void FStaticMeshBuilder::FillIndices(const uint32* Content, uint32 Size, uint32 Offset)
{
	StaticMesh->IndexBuffer.FillIndices(Content, Size, Offset);
}

Ref<FStaticMesh> FStaticMeshBuilder::Build(bool bComputeNormal, bool bComputeTangent)
{
	if (bComputeNormal)
	{
		ComputeNormals();
	}
	if (bComputeTangent)
	{
		ComputeTangents();
	}

	if (StaticMesh->Sections.size() == 0 && !StaticMesh->Materials.empty())
	{
		StaticMesh->Sections.push_back({0, 0, StaticMesh->IndexBuffer.GetNumTriangles()});
	}

	StaticMesh->VertexBuffer.ReleaseResource();
	StaticMesh->IndexBuffer.ReleaseResource();

	StaticMesh->VertexBuffer.InitResource();
	StaticMesh->IndexBuffer.InitResource();

	return StaticMesh;
}

bool FStaticMeshBuilder::GetVertexIndices(uint32 TriIndex, uint32& A, uint32& B, uint32& C) const
{
	if (TriIndex >= StaticMesh->IndexBuffer.GetNumTriangles())
		return false;

	A = StaticMesh->IndexBuffer.GetIndex(TriIndex * 3);
	B = StaticMesh->IndexBuffer.GetIndex(TriIndex * 3 + 1);
	C = StaticMesh->IndexBuffer.GetIndex(TriIndex * 3 + 2);

	return true;
}

Math::FVector4 FStaticMeshBuilder::ComputeTanget(uint32 A, uint32 B, uint32 C) const
{
	Math::FVector3 ba = StaticMesh->VertexBuffer.GetPosition(B) - StaticMesh->VertexBuffer.GetPosition(A);
	Math::FVector3 ca = StaticMesh->VertexBuffer.GetPosition(C) - StaticMesh->VertexBuffer.GetPosition(A);

	Math::FVector2 tba = StaticMesh->VertexBuffer.GetTexCoord(B) - StaticMesh->VertexBuffer.GetTexCoord(A);
	Math::FVector2 tca = StaticMesh->VertexBuffer.GetTexCoord(C) - StaticMesh->VertexBuffer.GetTexCoord(A);

	Math::FMatrix2 texMatrix = Math::FMatrix2(tba, tca);
	texMatrix.Invert();

	Math::FVector3 tangent, binormal;

	tangent = ba * texMatrix[0][0] + ca * texMatrix[0][1];
	binormal = ba * texMatrix[1][0] + ca * texMatrix[1][1];

	Math::FVector3 normal = Math::FVector3::Cross(ba, ca);
	Math::FVector3 biCross = Math::FVector3::Cross(tangent, normal);

	float handedness = 1.0f;
	if (Math::FVector3::Dot(biCross, binormal) < 0.0f)
	{
		handedness = -1.0f;
	}

	return Math::FVector4(tangent.X, tangent.Y, tangent.Z, handedness);
}

void FStaticMeshBuilder::ComputeTangents()
{
	if (!StaticMesh->IndexBuffer.IsIndexDataValid() || !StaticMesh->VertexBuffer.IsPositonDataValid() || !StaticMesh->VertexBuffer.IsTexCoordDataValid(0))
	{
		return;
	}

	StaticMesh->VertexBuffer.TangentData.Allocate<Math::FVector4>(StaticMesh->VertexBuffer.NumVertices);
	Math::FVector4* Tangents = StaticMesh->VertexBuffer.TangentData.DataAs<Math::FVector4>();

	uint32 TriCount = StaticMesh->IndexBuffer.GetNumTriangles();
	for (uint32 i = 0; i < TriCount; i++)
	{
		uint32 a = 0, b = 0, c = 0;
		GetVertexIndices(i, a, b, c);
		Math::FVector4 tangent = ComputeTanget(a, b, c);
		Tangents[a] += tangent;
		Tangents[b] += tangent;
		Tangents[c] += tangent;
	}
	for (uint32 i = 0; i < StaticMesh->VertexBuffer.GetNumVertices(); i++)
	{
		float handedness = Tangents[i].W > 0.0f ? 1.0f : -1.0f;
		Tangents[i].W = 0.0f;
		Tangents[i].Normalize();
		Tangents[i].W = handedness;
	}
}

void FStaticMeshBuilder::ComputeNormals()
{
	if (!StaticMesh->IndexBuffer.IsIndexDataValid() || !StaticMesh->VertexBuffer.IsPositonDataValid())
	{
		return;
	}
	StaticMesh->VertexBuffer.NormalData.Allocate<Math::FVector3>(StaticMesh->VertexBuffer.NumVertices);
	Math::FVector3* Normals = StaticMesh->VertexBuffer.NormalData.DataAs<Math::FVector3>();
	const Math::FVector3* Positions = StaticMesh->VertexBuffer.PositionData.DataAs<Math::FVector3>();

	uint32 TriCount = StaticMesh->IndexBuffer.GetNumTriangles();
	for (uint32 i = 0; i < TriCount; i++)
	{
		uint32 a = 0, b = 0, c = 0;
		GetVertexIndices(i, a, b, c);
		Math::FVector3 normal = Math::FVector3::Cross((Positions[b] - Positions[a]), (Positions[c] - Positions[a]));
		Normals[a] += normal;
		Normals[b] += normal;
		Normals[c] += normal;
	}
	for (uint32 i = 0; i < StaticMesh->VertexBuffer.GetNumVertices(); i++)
	{
		Normals[i].Normalize();
	}
}

}