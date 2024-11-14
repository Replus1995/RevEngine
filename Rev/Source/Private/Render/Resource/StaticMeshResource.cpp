#include "Rev/Render/Resource/StaticMeshResource.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RenderUtils.h"

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

	Cleanup();
}

void FStaticMeshVertexBuffer::Cleanup()
{
	PositionData.Release();
	ColorData.Release();
	NormalData.Release();
	TangentData.Release();
	for (uint32 i = 0; i < REV_MAX_TEXCOORDS; i++)
	{
		TexCoordData[i].Release();
	}
}

FRHIBuffer* FStaticMeshVertexBuffer::GetTexCoordBufferRHI(uint32 UVIndex) const
{
	REV_CORE_ASSERT(UVIndex < REV_MAX_TEXCOORDS);
	return TexCoordBuffer[UVIndex].VertexBufferRHI.get();
}

void FStaticMeshVertexBuffer::InitRHI()
{
	if(PositionData.Size() >= NumVertices * sizeof(Math::FVector3))
		PositionBuffer.VertexBufferRHI = CreateBufferFromData(PositionData, NumVertices * sizeof(Math::FVector3),sizeof(Math::FVector3));
	if (ColorData.Size() >= NumVertices * sizeof(Math::FColor))
		ColorBuffer.VertexBufferRHI = CreateBufferFromData(ColorData, NumVertices * sizeof(Math::FColor), sizeof(Math::FColor));
	if (NormalData.Size() >= NumVertices * sizeof(Math::FVector3))
		NormalBuffer.VertexBufferRHI = CreateBufferFromData(NormalData, NumVertices * sizeof(Math::FVector3), sizeof(Math::FVector3));
	if (TangentData.Size() >= NumVertices * sizeof(Math::FVector4))
		TangentBuffer.VertexBufferRHI = CreateBufferFromData(TangentData, NumVertices * sizeof(Math::FVector4), sizeof(Math::FVector4));
	for (uint32 i = 0; i < NumTexCoords; i++)
	{
		if (TexCoordData[i].Size() >= NumVertices * sizeof(Math::FVector2))
			TexCoordBuffer[i].VertexBufferRHI = CreateBufferFromData(TexCoordData[i], NumVertices * sizeof(Math::FVector2),  sizeof(Math::FVector2));
	}
}

void Rev::FStaticMeshVertexBuffer::ReleaseRHI()
{
	PositionBuffer.ReleaseRHI();
	ColorBuffer.ReleaseRHI();
	NormalBuffer.ReleaseRHI();
	TangentBuffer.ReleaseRHI();
	for (uint32 i = 0; i < REV_MAX_TEXCOORDS; i++)
	{
		TexCoordBuffer[i].ReleaseRHI();
	}
}

void FStaticMeshVertexBuffer::UpdateVertexStreams(FRHICommandList& RHICmdList) const
{
	if (!PositionBuffer.VertexBufferRHI)
		return;

	RHICmdList.GetContext()->RHISetVertexStream(0, PositionBuffer.VertexBufferRHI.get());
	RHICmdList.GetContext()->RHISetVertexStream(1, ColorBuffer.VertexBufferRHI ? ColorBuffer.VertexBufferRHI.get() : GNullVertexBuffer.VertexBufferRHI.get());
	RHICmdList.GetContext()->RHISetVertexStream(2, NormalBuffer.VertexBufferRHI ? NormalBuffer.VertexBufferRHI.get() : GNullVertexBuffer.VertexBufferRHI.get());
	RHICmdList.GetContext()->RHISetVertexStream(3, TangentBuffer.VertexBufferRHI ? TangentBuffer.VertexBufferRHI.get() : GNullVertexBuffer.VertexBufferRHI.get());
	for (uint32 i = 0; i < 2; i++)
	{
		RHICmdList.GetContext()->RHISetVertexStream(4 + i, TexCoordBuffer[i].VertexBufferRHI ? TexCoordBuffer[i].VertexBufferRHI.get() : GNullVertexBuffer.VertexBufferRHI.get());
	}
}

Math::FVector3 FStaticMeshVertexBuffer::GetPosition(uint32 VertexIndex) const
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	if (VertexIndex * sizeof(Math::FVector3) >= PositionData.Size())
		return Math::FVector3();
	return PositionData.DataAs<Math::FVector3>()[VertexIndex];
}

Math::FColor FStaticMeshVertexBuffer::GetColor(uint32 VertexIndex) const
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	if (VertexIndex * sizeof(Math::FColor) >= ColorData.Size())
		return Math::FColor();
	return ColorData.DataAs<Math::FColor>()[VertexIndex];
}

Math::FVector3 FStaticMeshVertexBuffer::GetNormal(uint32 VertexIndex) const
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	if (VertexIndex * sizeof(Math::FVector3) >= NormalData.Size())
		return Math::FVector3();
	return NormalData.DataAs<Math::FVector3>()[VertexIndex];
}

Math::FVector4 FStaticMeshVertexBuffer::GetTangent(uint32 VertexIndex) const
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	if (VertexIndex * sizeof(Math::FVector4) >= NormalData.Size())
		return Math::FVector4();
	return TangentData.DataAs<Math::FVector4>()[VertexIndex];
}

Math::FVector2 FStaticMeshVertexBuffer::GetTexCoord(uint32 VertexIndex, uint32 UVIndex) const
{
	REV_CORE_ASSERT(VertexIndex < NumVertices);
	REV_CORE_ASSERT(UVIndex < NumTexCoords);
	if (VertexIndex * sizeof(Math::FVector2) >= NormalData.Size())
		return Math::FVector2();
	return TexCoordData[UVIndex].DataAs<Math::FVector2>()[VertexIndex];
}

void FStaticMeshVertexBuffer::FillPositions(const float* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= NumVertices * 3);
	CheckAllocateData<Math::FVector3>(PositionData);
	memcpy(PositionData.DataAs<float>() + Offset, Content, Size * sizeof(float));
}

void FStaticMeshVertexBuffer::FillColors(const uint8* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= NumVertices * 4);
	CheckAllocateData<Math::FColor>(ColorData);
	memcpy(ColorData.DataAs<uint8>() + Offset, Content, Size * sizeof(uint8));
}

void FStaticMeshVertexBuffer::FillNormals(const float* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= NumVertices * 3);
	CheckAllocateData<Math::FVector3>(NormalData);
	memcpy(NormalData.DataAs<float>() + Offset, Content, Size * sizeof(float));
}

void FStaticMeshVertexBuffer::FillTangents(const float* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= NumVertices * 4);
	CheckAllocateData<Math::FVector4>(TangentData);
	memcpy(TangentData.DataAs<float>() + Offset, Content, Size * sizeof(float));
}

void FStaticMeshVertexBuffer::FillTexCoords(uint32 UVIndex, const float* Content, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(UVIndex < NumTexCoords);
	REV_CORE_ASSERT(Size + Offset <= NumVertices * 2);
	CheckAllocateData<Math::FVector2>(TexCoordData[UVIndex]);
	memcpy(TexCoordData[UVIndex].DataAs<float>() + Offset, Content, Size * sizeof(float));
}

Ref<FRHIBuffer> FStaticMeshVertexBuffer::CreateBufferFromData(const FBuffer& InData, uint32 InSize, uint32 InStride)
{
	if(InData.Empty())
		return nullptr;

	Ref<FRHIBuffer> OutBuffer = GDynamicRHI->RHICreateBuffer({ InSize, InStride, EBufferUsageFlags::VertexBuffer });
	FRenderCore::GetMainContext()->RHIUpdateBufferData(OutBuffer.get(), InData.Data(), InSize);
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
	Cleanup();
}

void FStaticMeshIndexBuffer::Cleanup()
{
	IndexData.Release();
}

bool FStaticMeshIndexBuffer::IsIndexDataValid() const
{
	uint32 Stride = b32Bit ? sizeof(uint32) : sizeof(uint16);
	return IndexData.Size() >= NumIndices * Stride;
}

void FStaticMeshIndexBuffer::InitRHI()
{
	uint32 Stride = b32Bit ? sizeof(uint32) : sizeof(uint16);
	if (IndexData.Size() >= NumIndices * Stride)
	{
		IndexBuffer.IndexBufferRHI = GDynamicRHI->RHICreateBuffer({ NumIndices * Stride, Stride, EBufferUsageFlags::Static | EBufferUsageFlags::IndexBuffer });
		FRenderCore::GetMainContext()->RHIUpdateBufferData(IndexBuffer.IndexBufferRHI.get(), IndexData.Data(), NumIndices * Stride);
	}
}

void FStaticMeshIndexBuffer::ReleaseRHI()
{
	IndexBuffer.ReleaseRHI();
}

uint32 FStaticMeshIndexBuffer::GetIndex(uint32 At)
{
	REV_CORE_ASSERT(At < NumIndices);
	if (b32Bit)
	{
		if (At * sizeof(uint32) >= IndexData.Size())
			return 0;
		return IndexData.DataAs<uint32>()[At];
	}
	else
	{
		if (At * sizeof(uint16) >= IndexData.Size())
			return 0;
		return IndexData.DataAs<uint16>()[At];
	}
}

void FStaticMeshIndexBuffer::SetIndex(uint32 At, uint32 Value)
{
	REV_CORE_ASSERT(At < NumIndices);
	CheckAllocateData();
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
	REV_CORE_ASSERT(Size + Offset <= NumIndices);
	CheckAllocateData();
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
	REV_CORE_ASSERT(Size + Offset <= NumIndices);
	CheckAllocateData();
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

void FStaticMeshIndexBuffer::CheckAllocateData()
{
	if (b32Bit)
	{
		if (IndexData.Size() < NumIndices * sizeof(uint32))
			IndexData.Allocate<uint32>(NumIndices);
	}
	else
	{
		if (IndexData.Size() < NumIndices * sizeof(uint16))
			IndexData.Allocate<uint16>(NumIndices);
	}
}

}