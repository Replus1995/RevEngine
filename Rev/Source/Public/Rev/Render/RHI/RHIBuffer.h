#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIResource.h"
#include <vector>

namespace Rev
{

enum class EVertexElementType : uint8
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
	Mat3,
	Mat4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};
uint32 VertexElementSize(EVertexElementType type);
uint32 VertexComponentCount(EVertexElementType type);

struct FRHIVertexElement
{
	std::string Name;
	EVertexElementType Type;
	uint16 Location;
	uint32 Size;
	uint32 Offset;
	bool Normalized;

	FRHIVertexElement() = default;

	FRHIVertexElement(const std::string& InName, EVertexElementType InType, uint32 InLocation, bool InNormalized = false)
		: Name(InName), Type(InType), Location(InLocation), Size(VertexElementSize(InType)), Offset(0), Normalized(InNormalized)
	{
	}

	uint32 GetElementSize() const;
	uint32 GetComponentCount() const;
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
	/**
	* @brief Update vertex buffer data
	* @param Data : memory pointer
	* @param Size : memory size in bytes
	* @param Offset : memory offset in bytes
	*/
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset = 0) = 0;

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
	/**
	* @brief Update index buffer data
	* @param Data : memory pointer
	* @param Count : element count (size = count * stride)
	* @param Offset : element offset (byteoffset = offset * stride)
	*/
	virtual void UpdateSubData(const void* Data, uint32 Count, uint32 Offset = 0) = 0;

	uint32 GetStride() const { return mStride; };
	uint32 GetCount() const { return mCount; }
	uint32 GetCapacity() const { return mStride * mCount; };
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

class REV_API FRHIUniformBufferDynamic : public FRHIResource
{
public:
	FRHIUniformBufferDynamic(uint32 InSize) : mSize(InSize) {}
	virtual ~FRHIUniformBufferDynamic() = default;
	virtual uint32 UpdateSubData(const void* Data, uint32 Size) = 0;
	virtual void Clear() = 0;
protected:
	uint32 mSize;
};

}