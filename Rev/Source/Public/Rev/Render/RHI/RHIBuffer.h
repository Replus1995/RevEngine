#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include <vector>

namespace Rev
{

struct FRHIBufferDesc
{
	uint32 Size = 0;
	uint32 Stride = 0;
	EBufferUsageFlags Usage = EBufferUsageFlags::None;

	bool operator == (const FRHIBufferDesc& Other) const
	{
		bool bSame = Size == Other.Size &&
			Stride == Other.Stride &&
			Usage == Other.Usage;
		return bSame;
	}

	bool operator != (const FRHIBufferDesc& Other) const
	{
		return !(*this == Other);
	}
};

class REV_API FRHIBuffer : public FRHIResource
{
public:
	FRHIBuffer(const FRHIBufferDesc& InDesc) : Desc(InDesc) {}
	virtual ~FRHIBuffer() = default;

	uint32 GetSize() const { return Desc.Size; }
	uint32 GetStride() const { return Desc.Stride; }
	EBufferUsageFlags GetUsage() const { return Desc.Usage; }
private:
	FRHIBufferDesc Desc;
};

class REV_API FRHIUniformBuffer : public FRHIResource
{
public:
	FRHIUniformBuffer(uint32 InSize) : BufferSize(InSize) {}
	virtual ~FRHIUniformBuffer() = default;
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset = 0) = 0;

	uint32 GetSize() const { return BufferSize; }

protected:
	uint32 BufferSize = 0;
};

class REV_API FRHIDynamicUniformBuffer : public FRHIResource
{
public:
	FRHIDynamicUniformBuffer(uint32 InSize) : BufferSize(InSize) {}
	virtual ~FRHIDynamicUniformBuffer() = default;
	virtual void Resize(uint32 InSize) = 0;
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset = 0) = 0;

	uint32 GetSize() const { return BufferSize; }

protected:
	uint32 BufferSize = 0;
};

}