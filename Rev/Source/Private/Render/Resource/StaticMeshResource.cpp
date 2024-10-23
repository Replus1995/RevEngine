#include "Rev/Render/Resource/StaticMeshResource.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FStaticMeshVertexBuffer::FStaticMeshVertexBuffer()
{
}

FStaticMeshVertexBuffer::~FStaticMeshVertexBuffer()
{
}

void FStaticMeshVertexBuffer::Init(uint32 InNumVertices, uint32 InNumTexCoords)
{
	REV_CORE_ASSERT(InNumTexCoords <= REV_MAX_TEXCOORDS)

	NumVertices = InNumVertices;
	NumTexCoords = InNumTexCoords;

	PositionData.Allocate<Math::FVector3>(NumVertices);
	NormalData.Allocate<Math::FVector3>(NumVertices);
	TangentData.Allocate<Math::FVector4>(NumVertices);
	TexCoordData.Allocate<Math::FVector2>(NumVertices * NumTexCoords);
	ColorData.Allocate<Math::FColor>(NumVertices);
}

void FStaticMeshVertexBuffer::Cleanup()
{
	PositionData.Release();
	NormalData.Release();
	TangentData.Release();
	TexCoordData.Release();
	ColorData.Release();
}

void FStaticMeshVertexBuffer::SetPosition(uint32 VertexIndex, const Math::FVector3& InPosition)
{
	PositionData.DataAs<Math::FVector3>()[VertexIndex] = InPosition;
}

void FStaticMeshVertexBuffer::SetNormal(uint32 VertexIndex, const Math::FVector3& InNormal)
{
	NormalData.DataAs<Math::FVector3>()[VertexIndex] = InNormal;
}

void FStaticMeshVertexBuffer::SetTangent(uint32 VertexIndex, const Math::FVector4& InTangent)
{
	TangentData.DataAs<Math::FVector4>()[VertexIndex] = InTangent;
}

void FStaticMeshVertexBuffer::SetTexCoord(uint32 VertexIndex, uint32 UVIndex, const Math::FVector2& InTexCoord)
{
	if(UVIndex >= NumTexCoords)
		return;
	TexCoordData.DataAs<Math::FVector2>()[VertexIndex * NumTexCoords + UVIndex] = InTexCoord;
}

void FStaticMeshVertexBuffer::SetColor(uint32 VertexIndex, const Math::FColor& InColor)
{
	ColorData.DataAs<Math::FColor>()[VertexIndex] = InColor;
}

}