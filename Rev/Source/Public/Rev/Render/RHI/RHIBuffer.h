#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include <vector>

namespace Rev
{

struct FRHIVertexElement
{
	std::string Name;
	EVertexType Type;
	uint8 Location;
	uint8 Size;
	uint8 Num;
	uint32 Offset;
	bool Normalized;

	FRHIVertexElement() = default;

	FRHIVertexElement(const std::string& InName, EVertexType InType, uint8 InLocation, uint32 InOffset = 0, bool InNormalized = false)
		: Name(InName), Type(InType), Location(InLocation), Offset(InOffset), Normalized(InNormalized)
	{
		Size = ElementSize(InType);
		Num = ComponentCount(InType);
	}
 

private:
	static uint8 ElementSize(EVertexType InType);
	static uint8 ComponentCount(EVertexType InType);
};

class FRHIVertexLayout
{
public:
	FRHIVertexLayout() {}

	FRHIVertexLayout(std::initializer_list<FRHIVertexElement> InElements)
		: mElements(InElements)
	{
		CalculateOffsetsAndStride();
	}

	uint32 GetStride() const { return mStride; }
	const std::vector<FRHIVertexElement>& GetElements() const { return mElements; }

	std::vector<FRHIVertexElement>::iterator begin() { return mElements.begin(); }
	std::vector<FRHIVertexElement>::iterator end() { return mElements.end(); }
	std::vector<FRHIVertexElement>::const_iterator begin() const { return mElements.begin(); }
	std::vector<FRHIVertexElement>::const_iterator end() const { return mElements.end(); }
private:
	void CalculateOffsetsAndStride()
	{
		mStride = 0;
		for (auto& element : mElements)
		{
			element.Offset = mStride;
			mStride += element.Size;
		}
	}
private:
	std::vector<FRHIVertexElement> mElements;
	uint32 mStride = 0;
};

class REV_API FRHIVertexBuffer : public FRHIResource
{
public:
	FRHIVertexBuffer(uint32 InSize) : mSize(InSize) {}
	virtual ~FRHIVertexBuffer() = default;

	const FRHIVertexLayout& GetLayout() const { return mLayout; }
	void SetLayout(const FRHIVertexLayout& InLayout) { mLayout = InLayout; }
	uint32 GetCapacity() const { return mSize; }
protected:
	uint32 mSize;
	FRHIVertexLayout mLayout;
};

class REV_API FRHIIndexBuffer : public FRHIResource
{
public:
	FRHIIndexBuffer(uint32 InStride, uint32 InCount) : mStride(InStride), mCount(InCount) {}
	virtual ~FRHIIndexBuffer() = default;

	uint32 GetStride() const { return mStride; }
	uint32 GetCount() const { return mCount; }
	uint32 GetCapacity() const { return mStride * mCount; }
protected:
	uint32 mStride = 0;
	uint32 mCount = 0;
};

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