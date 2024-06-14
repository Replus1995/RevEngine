#pragma once
#include "Pine/Render/Resource/VertexBuffer.h"

namespace Pine
{

class OpenGLVertexBuffer : public VertexBuffer
{
public:
	OpenGLVertexBuffer(uint32_t size);
	OpenGLVertexBuffer(float* vertices, uint32_t size);
	virtual ~OpenGLVertexBuffer();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void SetData(const void* data, uint32_t size) override;

	virtual const BufferLayout& GetLayout() const override { return mLayout; }
	virtual void SetLayout(const BufferLayout& layout) override { mLayout = layout; }
private:
	uint32_t mHandle = 0;
	BufferLayout mLayout;
};

class OpenGLIndexBuffer : public IndexBuffer
{
public:
	OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
	virtual ~OpenGLIndexBuffer();

	virtual void Bind() const;
	virtual void Unbind() const;

	virtual uint32_t GetCount() const { return mCount; }
private:
	uint32_t mHandle = 0;
	uint32_t mCount = 0;
};

class OpenGLVertexArray : public VertexArray
{
public:
	OpenGLVertexArray();
	virtual ~OpenGLVertexArray();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
	virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

	virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return mVertexBuffers; }
	virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return mIndexBuffer; }
private:
	uint32_t mHandle = 0;
	uint32_t mVertexBufferIndex = 0;
	std::vector<std::shared_ptr<VertexBuffer>> mVertexBuffers;
	std::shared_ptr<IndexBuffer> mIndexBuffer;
};

}