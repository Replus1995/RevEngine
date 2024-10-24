#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Resource/RenderResource.h"

namespace Rev
{

struct FStaticMeshVertexSimple
{
	Math::FVector3 Position;
	Math::FVector2 TexCoords[REV_MAX_TEXCOORDS];
	Math::FColor Color;
};

struct FStaticMeshVertex
{
	Math::FVector3 Position;
	Math::FVector3 Normal;
	Math::FVector4 Tangent;
	Math::FVector2 TexCoords[REV_MAX_TEXCOORDS];
	Math::FColor Color;
};

class REV_API FStaticMeshVertexBuffer : public FRenderResource
{
	friend class FStaticMeshBuilder;
public:
	FStaticMeshVertexBuffer();
	virtual ~FStaticMeshVertexBuffer();

	void Init(uint32 InNumVertices, uint32 InNumTexCoords);
	void Cleanup();

	FORCEINLINE uint32 GetNumVertices() const { return NumVertices; }
	FORCEINLINE uint32 GetNumTexCoords() const { return NumTexCoords; }

	FRHIBuffer* GetPositionBufferRHI() const { return PositionBuffer.VertexBufferRHI.get(); }
	FRHIBuffer* GetNormalBufferRHI() const { return NormalBuffer.VertexBufferRHI.get(); }
	FRHIBuffer* GetTangentBufferRHI() const { return TangentBuffer.VertexBufferRHI.get(); }
	FRHIBuffer* GetTexCoordBufferRHI() const { return TexCoordBuffer.VertexBufferRHI.get(); }
	FRHIBuffer* GetColorBufferRHI() const { return ColorBuffer.VertexBufferRHI.get(); }

	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;

protected:
	Math::FVector3& GetPosition(uint32 VertexIndex);
	Math::FVector3& GetNormal(uint32 VertexIndex);
	Math::FVector4& GetTangent(uint32 VertexIndex);
	Math::FVector2& GetTexCoord(uint32 VertexIndex, uint32 UVIndex = 0);
	Math::FColor& GetColor(uint32 VertexIndex);

	void FillPositions(const float* Content, uint32 Size, uint32 Offset);
	void FillNormals(const float* Content, uint32 Size, uint32 Offset);
	void FillTangents(const float* Content, uint32 Size, uint32 Offset);
	void FillTexCoords(uint32 UVIndex, const float* Content, uint32 Size, uint32 Offset);
	void FillColors(const uint8* Content, uint32 Size, uint32 Offset);

	Ref<FRHIBuffer> CreateBufferFromData(const FBuffer& InData, uint32 InStride);

protected:
	uint32 NumVertices = 0;
	uint32 NumTexCoords = 0;

	//CPU Data
	FBuffer PositionData;
	FBuffer NormalData;
	FBuffer TangentData;
	FBuffer TexCoordData;
	FBuffer ColorData;

	//GPU Data
	FVertexBuffer PositionBuffer;
	FVertexBuffer NormalBuffer; //can be optimized by geometry shader
	FVertexBuffer TangentBuffer;
	FVertexBuffer TexCoordBuffer;
	FVertexBuffer ColorBuffer;
};

class REV_API FStaticMeshIndexBuffer : public FRenderResource
{
	friend class FStaticMeshBuilder;
public:
	FStaticMeshIndexBuffer();
	virtual ~FStaticMeshIndexBuffer();

	void Init(uint32 InNumIndices, bool bUse32Bit);
	void Cleanup();

	FORCEINLINE uint32 GetNumIndices() const { return NumIndices; }
	FORCEINLINE uint32 GetNumTriangles() const { return NumIndices / 3; }
	FORCEINLINE bool Is32Bit() const { return b32Bit; }
	FRHIBuffer* GetIndexBufferRHI() const { return IndexBuffer.IndexBufferRHI.get(); }

	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;

protected:
	uint32 GetIndex(uint32 At);
	void SetIndex(uint32 At, uint32 Value);

	void FillIndices(const uint16* Content, uint32 Size, uint32 Offset);
	void FillIndices(const uint32* Content, uint32 Size, uint32 Offset);

protected:
	uint32 NumIndices = 0;
	bool b32Bit = false;

	FBuffer IndexData;
	FIndexBuffer IndexBuffer;
};


}