#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/Mesh/MeshPrimitive.h"

namespace Rev
{

struct FMeshPrimitiveStorage
{
public:
	int MaterialIndex = 0;
	EPrimitiveTopology PrimitiveTopology = PT_Triangles;

	uint32 VertexCount = 0;
	FBuffer PositonData;	//size = VertexCount * 3 * sizeof(float)
	FBuffer NormalData;		//size = VertexCount * 3 * sizeof(float)
	FBuffer TangentData;	//size = VertexCount * 4 * sizeof(float)
	FBuffer TexCoordData;	//size = VertexCount * 2 * sizeof(float)

	//FBuffer WeightData; //size = VertexCount * 4 * sizeof(float)
	//FBuffer WeightIndexData; //size = VertexCount * 4 * sizeof(int)

	uint32 IndexCount = 0;
	uint32 IndexStride = 0;
	FBuffer IndexData; //size = IndexCount * IndexStride

	FMeshPrimitiveStorage();
	FMeshPrimitiveStorage(const FMeshPrimitiveStorage&) = delete;
	FMeshPrimitiveStorage(FMeshPrimitiveStorage&& InStorage) noexcept;
	~FMeshPrimitiveStorage();

	FMeshPrimitive CreateVertexData();
protected:
	uint32 NumTriangles() const;
	bool GetVertexIndices(uint32 TriIndex, uint32& A, uint32& B, uint32& C) const;

	void CalculateNormals();
	Math::FVector4 CalculateTangent(uint32 a, uint32 b, uint32 c) const;
	void CalculateTangents();
};

}