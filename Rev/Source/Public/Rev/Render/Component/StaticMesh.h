#pragma once
#include "Rev/Render/Resource/StaticMeshResource.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{

struct FStaticMeshSection
{
	int32 MaterialIndex = -1;

	uint32 StartIndex = 0;
	uint32 NumTriangles = 0;
};

class REV_API FStaticMesh
{
	friend class FStaticMeshBuilder;
	friend class FStaticMeshProxy;
public:
	FStaticMesh();
	virtual ~FStaticMesh();

	bool IsEmpty() const;
	const FStaticMeshVertexBuffer& GetVertexBuffer() const { return VertexBuffer; }
	const FStaticMeshIndexBuffer& GetIndexBuffer() const { return IndexBuffer; }
	void PrepareForDraw();
	bool IsReadyForDraw() const { return VertexBuffer.IsReadyForDraw() && IndexBuffer.IsReadyForDraw(); }

	uint32 GetNumMaterials() const { return Materials.size(); }
	FMaterial* GetMaterial(uint32 InMaterialIndex) const { return Materials[InMaterialIndex].get(); }

	uint32 GetNumSections() const { return Sections.size(); };
	const FStaticMeshSection* GetSection(uint32 InSectionIndex) const { return &Sections[InSectionIndex]; }
	std::vector<const FStaticMeshSection*> GetSectionsForMaterial(uint32 InMaterialIndex) const;

protected:
	FStaticMeshVertexBuffer VertexBuffer;
	FStaticMeshIndexBuffer IndexBuffer;
	std::vector<Ref<FMaterial>> Materials;
	std::vector<FStaticMeshSection> Sections;
};

class REV_API FStaticMeshBuilder
{
public:
	FStaticMeshBuilder();
	virtual ~FStaticMeshBuilder();

	void Init(uint32 InNumVertices, uint32 InNumTexCoords, uint32 InNumIndices, bool bUse32Bit);
	void InitVertices(uint32 InNumVertices, uint32 InNumTexCoords, 
		FBuffer&& InPositionData, 
		FBuffer&& InNormalData, 
		FBuffer&& InTangentData,
		FBuffer* InTexCoordData,
		FBuffer&& InColorData
		);
	void InitIndices(uint32 InNumIndices, bool bUse32Bit, FBuffer&& InIndexData);


	void SetMaterials(const std::vector<Ref<FMaterial>>& InMaterials);

	void FillPositions(const float* Content, uint32 Size, uint32 Offset = 0);
	void FillNormals(const float* Content, uint32 Size, uint32 Offset = 0);
	void FillTangents(const float* Content, uint32 Size, uint32 Offset = 0);
	void FillTexCoords(uint32 UVIndex, const float* Content, uint32 Size, uint32 Offset = 0);
	void FillColors(const uint8* Content, uint32 Size, uint32 Offset = 0);

	void FillIndices(const uint16* Content, uint32 Size, uint32 Offset = 0);
	void FillIndices(const uint32* Content, uint32 Size, uint32 Offset = 0);

	Ref<FStaticMesh> Build(bool bComputeNormal, bool bComputeTangent);

protected:
	bool GetVertexIndices(uint32 TriIndex, uint32& A, uint32& B, uint32& C) const;

	Math::FVector4 ComputeTanget(uint32 A, uint32 B, uint32 C) const;
	void ComputeTangents();
	void ComputeNormals();

protected:
	Ref<FStaticMesh> StaticMesh;
	bool bMeshDataDirty = false;
};

}