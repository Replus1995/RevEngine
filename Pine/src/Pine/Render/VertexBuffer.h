#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Render/RenderCore.h"
#include <string>

namespace Pine
{
struct BufferElement
{
	std::string Name;
	EShaderDataType Type;
	uint32_t Size;
	size_t Offset;
	bool Normalized;

	BufferElement() = default;

	BufferElement(EShaderDataType type, const std::string& name, bool normalized = false)
		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
	{
	}

	uint32_t GetComponentCount() const;
};

class BufferLayout
{
public:
	BufferLayout() {}

	BufferLayout(std::initializer_list<BufferElement> elements)
		: mElements(elements)
	{
		CalculateOffsetsAndStride();
	}

	uint32_t GetStride() const { return mStride; }
	const std::vector<BufferElement>& GetElements() const { return mElements; }

	std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
	std::vector<BufferElement>::iterator end() { return mElements.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }
private:
	void CalculateOffsetsAndStride()
	{
		size_t offset = 0;
		mStride = 0;
		for (auto& element : mElements)
		{
			element.Offset = offset;
			offset += element.Size;
			mStride += element.Size;
		}
	}
private:
	std::vector<BufferElement> mElements;
	uint32_t mStride = 0;
};

class VertexBuffer
{
public:
	virtual ~VertexBuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void SetData(const void* data, uint32_t size) = 0;

	virtual const BufferLayout& GetLayout() const = 0;
	virtual void SetLayout(const BufferLayout& layout) = 0;

	static Ref<VertexBuffer> Create(uint32_t size);
	static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
};

class IndexBuffer
{
public:
	virtual ~IndexBuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual uint32_t GetCount() const = 0;

	static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
};

class VertexArray
{
public:
	virtual ~VertexArray() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
	virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

	virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
	virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

	static Ref<VertexArray> Create();
};

}