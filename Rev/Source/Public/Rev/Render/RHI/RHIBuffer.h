#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIResource.h"
#include <vector>

namespace Rev
{

enum class EVertexElementType
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

struct FVertexBufferElement
{
	std::string Name;
	EVertexElementType Type;
	uint16 Location;
	uint32 Size;
	uint32 Offset;
	bool Normalized;

	FVertexBufferElement() = default;

	FVertexBufferElement(const std::string& InName, EVertexElementType InType, uint32 InLocation, bool InNormalized = false)
		: Name(InName), Type(InType), Location(InLocation), Size(VertexElementSize(InType)), Offset(0), Normalized(InNormalized)
	{
	}

	uint32 GetElementSize() const;
	uint32 GetComponentCount() const;
};

class FVertexBufferLayout
{
public:
	FVertexBufferLayout() {}

	FVertexBufferLayout(std::initializer_list<FVertexBufferElement> InElements)
		: mElements(InElements)
	{
		CalculateOffsetsAndStride();
	}

	uint32 GetStride() const { return mStride; }
	const std::vector<FVertexBufferElement>& GetElements() const { return mElements; }

	std::vector<FVertexBufferElement>::iterator begin() { return mElements.begin(); }
	std::vector<FVertexBufferElement>::iterator end() { return mElements.end(); }
	std::vector<FVertexBufferElement>::const_iterator begin() const { return mElements.begin(); }
	std::vector<FVertexBufferElement>::const_iterator end() const { return mElements.end(); }
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
	std::vector<FVertexBufferElement> mElements;
	uint32 mStride = 0;
};

class REV_API FRHIVertexBuffer : public FRHIResource
{
public:
	virtual ~FRHIVertexBuffer() = default;
	/**
	* @brief Update vertex buffer data
	* @param Data : memory pointer
	* @param Size : memory size in bytes
	* @param Offset : memory offset in bytes
	*/
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset = 0) = 0;

	virtual const FVertexBufferLayout& GetLayout() const = 0;
	virtual void SetLayout(const FVertexBufferLayout& Layout) = 0;
	virtual uint32 GetCapacity() const = 0;
};

class REV_API FRHIIndexBuffer : public FRHIResource
{
public:
	virtual ~FRHIIndexBuffer() = default;
	/**
	* @brief Update index buffer data
	* @param Data : memory pointer
	* @param Count : element count (size = count * stride)
	* @param Offset : element offset (byteoffset = offset * stride)
	*/
	virtual void UpdateSubData(const void* Data, uint32 Count, uint32 Offset = 0) = 0;

	virtual uint32 GetStride() const = 0;
	virtual uint32 GetCount() const = 0;
	virtual uint32 GetCapacity() const = 0;
};

class REV_API FRHIVertexArray : public FRHIResource
{
public:
	virtual ~FRHIVertexArray() = default;
	virtual void AddVertexBuffer(const Ref<FRHIVertexBuffer>& VertexBuffer) = 0;
	virtual void SetIndexBuffer(const Ref<FRHIIndexBuffer>& IndexBuffer) = 0;

	virtual const std::vector<Ref<FRHIVertexBuffer>>& GetVertexBuffers() const = 0;
	virtual const Ref<FRHIIndexBuffer>& GetIndexBuffer() const = 0;
};

class REV_API FRHIUniformBuffer : public FRHIResource
{
public:
	virtual ~FRHIUniformBuffer() = default;
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset = 0) = 0;
};

}