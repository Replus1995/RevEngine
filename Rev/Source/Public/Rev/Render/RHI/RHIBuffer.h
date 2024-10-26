#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include <vector>

namespace Rev
{

class REV_API FRHIUniformBuffer : public FRHIResource
{
public:
	FRHIUniformBuffer(uint32 InSize) : mSize(InSize) {}
	virtual ~FRHIUniformBuffer() = default;
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset = 0) = 0;

	uint32 GetSize() const { return mSize; }

protected:
	uint32 mSize;
};

class FRHIBuffer : public FRHIResource
{
public:
	FRHIBuffer(uint32 InSize, uint32 InStride, EBufferUsageFlags InUsage) : Size(InSize), Stride(InStride), Usage(InUsage) {}

	uint32 GetSize() const { return Size; }
	uint32 GetStride() const { return Stride; }
	EBufferUsageFlags GetUsage() const { return Usage; }
private:
	uint32 Size = 0;
	uint32 Stride = 0;
	EBufferUsageFlags Usage = EBufferUsageFlags::None;
};

}