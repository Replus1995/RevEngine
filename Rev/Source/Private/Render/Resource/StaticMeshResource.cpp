#include "Rev/Render/Resource/StaticMeshResource.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RenderCore.h"

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

void FStaticMeshVertexBuffer::InitRHI()
{
	PositionBuffer.VertexBufferRHI = CreateBufferFromData(PositionData, sizeof(Math::FVector3));
	NormalBuffer.VertexBufferRHI = CreateBufferFromData(NormalData, sizeof(Math::FVector3));
	TangentBuffer.VertexBufferRHI = CreateBufferFromData(TangentData, sizeof(Math::FVector4));
	TexCoordBuffer.VertexBufferRHI = CreateBufferFromData(TexCoordData, sizeof(Math::FVector2) * NumTexCoords);
	ColorBuffer.VertexBufferRHI = CreateBufferFromData(ColorData, sizeof(Math::FColor));
}

void Rev::FStaticMeshVertexBuffer::ReleaseRHI()
{
	PositionBuffer.ReleaseRHI();
	NormalBuffer.ReleaseRHI();
	TangentBuffer.ReleaseRHI();
	TexCoordBuffer.ReleaseRHI();
	ColorBuffer.ReleaseRHI();
}

Math::FVector3& FStaticMeshVertexBuffer::GetPosition(uint32 VertexIndex)
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	return PositionData.DataAs<Math::FVector3>()[VertexIndex];
}

Math::FVector3& FStaticMeshVertexBuffer::GetNormal(uint32 VertexIndex)
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	return NormalData.DataAs<Math::FVector3>()[VertexIndex];
}

Math::FVector4& FStaticMeshVertexBuffer::GetTangent(uint32 VertexIndex)
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	return TangentData.DataAs<Math::FVector4>()[VertexIndex];
}

Math::FVector2& FStaticMeshVertexBuffer::GetTexCoord(uint32 VertexIndex, uint32 UVIndex)
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	REV_CORE_ASSERT(UVIndex < NumTexCoords);
	return TexCoordData.DataAs<Math::FVector2>()[VertexIndex * NumTexCoords + UVIndex];
}

Math::FColor& FStaticMeshVertexBuffer::GetColor(uint32 VertexIndex)
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	return ColorData.DataAs<Math::FColor>()[VertexIndex];
}

void FStaticMeshVertexBuffer::FillPositions(const float* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset < NumVertices * 3);
	memcpy(PositionData.DataAs<float>() + Offset, Content, Size * sizeof(float));
}

void FStaticMeshVertexBuffer::FillNormals(const float* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset < NumVertices * 3);
	memcpy(NormalData.DataAs<float>() + Offset, Content, Size * sizeof(float));
}

void FStaticMeshVertexBuffer::FillTangents(const float* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset < NumVertices * 4);
	memcpy(TangentData.DataAs<float>() + Offset, Content, Size * sizeof(float));
}

void FStaticMeshVertexBuffer::FillTexCoords(uint32 UVIndex, const float* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(UVIndex < NumTexCoords);
	REV_CORE_ASSERT(Size + Offset < NumVertices * 2);
	uint32 TexCoordStride = NumTexCoords * 2;
	float* TexCoordPtr = TexCoordData.DataAs<float>();
	for (uint32 i = 0; i < Size; i++)
	{
		uint32 DstIndex = (Offset + i) / 2 * TexCoordStride + (Offset + i) % 2;
		TexCoordPtr[DstIndex] = Content[i];
	}
}

void FStaticMeshVertexBuffer::FillColors(const uint8* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset < NumVertices * 4);
	memcpy(ColorData.DataAs<uint8>() + Offset, Content, Size * sizeof(uint8));
}

Ref<FRHIBuffer> FStaticMeshVertexBuffer::CreateBufferFromData(const FBuffer& InData, uint32 InStride)
{
	if(InData.Empty())
		return nullptr;

	Ref<FRHIBuffer> OutBuffer = GDynamicRHI->RHICreateBuffer(InData.Size(), InStride, EBufferUsageFlags::VertexBuffer);
	FRenderCore::GetMainContext()->UpdateBufferData(OutBuffer.get(), InData.Data(), InData.Size());
	return OutBuffer;
}

FStaticMeshIndexBuffer::FStaticMeshIndexBuffer()
{
}

FStaticMeshIndexBuffer::~FStaticMeshIndexBuffer()
{
}

void FStaticMeshIndexBuffer::Init(uint32 InNumIndices, bool bUse32Bit)
{
	NumIndices = InNumIndices;
	b32Bit = bUse32Bit;

	if (b32Bit)
	{
		IndexData.Allocate<uint32>(NumIndices);
	}
	else
	{
		IndexData.Allocate<uint16>(NumIndices);
	}
}

void FStaticMeshIndexBuffer::Cleanup()
{
	IndexData.Release();
}

void FStaticMeshIndexBuffer::InitRHI()
{
	if (!IndexData.Empty())
	{
		uint32 Stride = b32Bit ? sizeof(uint32) : sizeof(uint16);
		IndexBuffer.IndexBufferRHI = GDynamicRHI->RHICreateBuffer(IndexData.Size(), Stride, EBufferUsageFlags::Static | EBufferUsageFlags::IndexBuffer);
		FRenderCore::GetMainContext()->UpdateBufferData(IndexBuffer.IndexBufferRHI.get(), IndexData.Data(), IndexData.Size());
	}
}

void FStaticMeshIndexBuffer::ReleaseRHI()
{
	IndexBuffer.ReleaseRHI();
}

uint32 FStaticMeshIndexBuffer::GetIndex(uint32 At)
{
	if (b32Bit)
	{
		return IndexData.DataAs<uint32>()[At];
	}
	else
	{
		return IndexData.DataAs<uint16>()[At];
	}
}

void FStaticMeshIndexBuffer::SetIndex(uint32 At, uint32 Value)
{
	if (b32Bit)
	{
		IndexData.DataAs<uint32>()[At] = Value;
	}
	else
	{
		IndexData.DataAs<uint16>()[At] = (uint16)Value;
	}
}

void FStaticMeshIndexBuffer::FillIndices(const uint16* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset < NumIndices);
	if (b32Bit)
	{
		for (uint32 i = 0; i < Size; i++)
		{
			SetIndex(Offset + i, Content[i]);
		}
	}
	else
	{
		memcpy(IndexData.DataAs<uint16>() + Offset, Content, Size * sizeof(uint16));
	}
}

void FStaticMeshIndexBuffer::FillIndices(const uint32* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset < NumIndices);
	if (b32Bit)
	{
		memcpy(IndexData.DataAs<uint32>() + Offset, Content, Size * sizeof(uint32));
	}
	else
	{
		for (uint32 i = 0; i < Size; i++)
		{
			SetIndex(Offset + i, Content[i]);
		}
	}
}

}