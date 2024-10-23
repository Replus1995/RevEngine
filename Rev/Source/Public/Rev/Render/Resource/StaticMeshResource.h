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
public:
	FStaticMeshVertexBuffer();
	virtual ~FStaticMeshVertexBuffer();

	void Init(uint32 InNumVertices, uint32 InNumTexCoords);
	void Cleanup();

	void SetPosition(uint32 VertexIndex, const Math::FVector3& InPosition);
	void SetNormal(uint32 VertexIndex, const Math::FVector3& InNormal);
	void SetTangent(uint32 VertexIndex, const Math::FVector4& InTangent);
	void SetTexCoord(uint32 VertexIndex, uint32 UVIndex, const Math::FVector2& InTexCoord);
	void SetColor(uint32 VertexIndex, const Math::FColor& InColor);


	virtual void InitRHI() override;
	virtual void ReleaseRHI() override;

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

struct FStaticMeshSection
{

};

}