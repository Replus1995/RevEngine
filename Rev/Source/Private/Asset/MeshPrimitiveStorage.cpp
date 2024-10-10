#include "Rev/Asset/MeshPrimitiveStorage.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHICore.h"

namespace Rev
{

FMeshPrimitiveStorage::FMeshPrimitiveStorage()
{
}

FMeshPrimitiveStorage::FMeshPrimitiveStorage(FMeshPrimitiveStorage&& InStorage) noexcept
{
	MaterialIndex = InStorage.MaterialIndex;
	PrimitiveTopology = InStorage.PrimitiveTopology;

	VertexCount = InStorage.VertexCount;
	PositonData = std::move(InStorage.PositonData);
	NormalData = std::move(InStorage.NormalData);
	TangentData = std::move(InStorage.TangentData);
	TexCoordData = std::move(InStorage.TexCoordData);

	//WeightData = std::move(InStorage.WeightData);
	//WeightIndexData = std::move(InStorage.WeightIndexData);

	IndexCount = InStorage.IndexCount;
	IndexType = InStorage.IndexType;
	IndexData = std::move(InStorage.IndexData);
}

FMeshPrimitiveStorage::~FMeshPrimitiveStorage()
{
}

FMeshPrimitive FMeshPrimitiveStorage::CreateVertexData()
{
	if(VertexCount == 0 || IndexCount == 0 || TexCoordData.Empty())
		return {};
	if (NormalData.Empty())
		CalculateNormals();
	if (TangentData.Empty())
		CalculateTangents();

	Ref<FRHIPrimitive> Primitive = FRHICore::CreatePrimitive(PT_Triangles);
	//Position
	{
		Ref<FRHIVertexBuffer> PositionBuffer = FRHICore::CreateVertexBuffer(PositonData.DataAs<float>(), PositonData.Size());
		PositionBuffer->SetLayout({
			{"Position", EVertexElementType::Float3,  0}
			});
		Primitive->AddVertexBuffer(PositionBuffer);
	}
	//Normal
	{
		Ref<FRHIVertexBuffer> NormalBuffer = FRHICore::CreateVertexBuffer(NormalData.DataAs<float>(), NormalData.Size());
		NormalBuffer->SetLayout({
			{"Normal", EVertexElementType::Float3,  1}
			});
		Primitive->AddVertexBuffer(NormalBuffer);
	}
	//Tangent
	{
		Ref<FRHIVertexBuffer> TangentBuffer = FRHICore::CreateVertexBuffer(TangentData.DataAs<float>(), TangentData.Size());
		TangentBuffer->SetLayout({
			{"Tangent", EVertexElementType::Float4,  2}
			});
		Primitive->AddVertexBuffer(TangentBuffer);
	}
	//TexCoord
	{
		Ref<FRHIVertexBuffer> TexCoordBuffer = FRHICore::CreateVertexBuffer(TexCoordData.DataAs<float>(), TexCoordData.Size());
		TexCoordBuffer->SetLayout({
			{"TexCoord0", EVertexElementType::Float2,  3}
			});
		Primitive->AddVertexBuffer(TexCoordBuffer);
	}

	Ref<FRHIIndexBuffer> IndexBuffer = FRHICore::CreateIndexBuffer(IndexData.Data(), IndexType, IndexCount);
	Primitive->SetIndexBuffer(IndexBuffer);

	FMeshPrimitive Result;
	Result.MaterialIndex = MaterialIndex;
	Result.PrimitiveData = Primitive;
	return Result;
}

uint32 FMeshPrimitiveStorage::NumTriangles() const
{
	return IndexCount / 3;
}

bool FMeshPrimitiveStorage::GetVertexIndices(uint32 TriIndex, uint32& A, uint32& B, uint32& C) const
{
	if(TriIndex >= NumTriangles())
		return false;

	switch (IndexType)
	{
	case EIndexElementType::UInt16:
	{
		const uint16* Indices = IndexData.DataAs<uint16>();
		A = Indices[TriIndex * 3];
		B = Indices[TriIndex * 3 + 1];
		C = Indices[TriIndex * 3 + 2];
		break;
	}
	case EIndexElementType::UInt32:
	{
		const uint32* Indices = IndexData.DataAs<uint32>();
		A = Indices[TriIndex * 3];
		B = Indices[TriIndex * 3 + 1];
		C = Indices[TriIndex * 3 + 2];
		break;
	}
	default:
		REV_CORE_ASSERT(false, "Unknow index type.");
		break;
	}

	return true;
}

void FMeshPrimitiveStorage::CalculateNormals()
{
	if (!NormalData.Empty() || VertexCount == 0 || IndexCount == 0)
	{
		return;
	}
	NormalData.Allocate(VertexCount * sizeof(Math::FVector3));
	memset(NormalData.Data(), 0, NormalData.Size());

	Math::FVector3* Normals = NormalData.DataAs<Math::FVector3>();
	const Math::FVector3* Positions = PositonData.DataAs<Math::FVector3>();

	uint32 TriCount = NumTriangles();
	for (uint32 i = 0; i < TriCount; i++)
	{
		uint32 a = 0, b = 0, c = 0;
		GetVertexIndices(i, a, b, c);
		Math::FVector3 normal = Math::FVector3::Cross((Positions[b] - Positions[a]), (Positions[c] - Positions[a]));
		Normals[a] += normal;
		Normals[b] += normal;
		Normals[c] += normal;
	}
	for (uint32 i = 0; i < VertexCount; i++)
	{
		Normals[i].Normalize();
	}
}

Math::FVector4 FMeshPrimitiveStorage::CalculateTangent(uint32 a, uint32 b, uint32 c) const
{
	const Math::FVector3* Positions = PositonData.DataAs<Math::FVector3>();
	const Math::FVector2* TexCoords = TexCoordData.DataAs<Math::FVector2>();

	Math::FVector3 ba = Positions[b] - Positions[a];
	Math::FVector3 ca = Positions[c] - Positions[a];

	Math::FVector2 tba = TexCoords[b] - TexCoords[a];
	Math::FVector2 tca = TexCoords[c] - TexCoords[a];

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

void FMeshPrimitiveStorage::CalculateTangents()
{
	if (!TangentData.Empty() || VertexCount == 0 || IndexCount == 0 || TexCoordData.Empty())
	{
		return;
	}
	TangentData.Allocate(VertexCount * sizeof(Math::FVector4));
	memset(TangentData.Data(), 0, TangentData.Size());

	Math::FVector4* Tangents = TangentData.DataAs<Math::FVector4>();

	uint32 TriCount = NumTriangles();
	for (uint32 i = 0; i < TriCount; i++)
	{
		unsigned int a = 0, b = 0, c = 0;
		GetVertexIndices(i, a, b, c);
		Math::FVector4 tangent = CalculateTangent(a, b, c);
		Tangents[a] += tangent;
		Tangents[b] += tangent;
		Tangents[c] += tangent;
	}
	for (uint32 i = 0; i < VertexCount; i++)
	{
		float handedness = Tangents[i].W > 0.0f ? 1.0f : -1.0f;
		Tangents[i].W = 0.0f;
		Tangents[i].Normalize();
		Tangents[i].W = handedness;
	}
}

}


