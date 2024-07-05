#pragma once
#include "Rev/Render/Resource/VertexBuffer.h"

namespace Rev
{

class FOpenGLVertexBuffer : public FVertexBuffer
{
public:
	FOpenGLVertexBuffer(uint32 size);
	FOpenGLVertexBuffer(const float* vertices, uint32 size);
	virtual ~FOpenGLVertexBuffer();

	virtual void Bind() const override;
	virtual void Unbind() const override;
	virtual void SetData(const void* data, uint32 size, uint32 offset) override;

	virtual const FVertexBufferLayout& GetLayout() const override { return mLayout; }
	virtual void SetLayout(const FVertexBufferLayout& layout) override { mLayout = layout; }
	virtual uint32 GetCapacity() const override { return mSize; };

private:
	uint32 mHandle = 0;
	uint32 mSize = 0;
	FVertexBufferLayout mLayout;
};

class FOpenGLIndexBuffer : public FIndexBuffer
{
public:
	FOpenGLIndexBuffer(uint32 stride, uint32 count);
	FOpenGLIndexBuffer(const void* indices, uint32 stride, uint32 count);
	virtual ~FOpenGLIndexBuffer();

	virtual void Bind() const override;
	virtual void Unbind() const override;
	virtual void SetData(const void* data, uint32 count, uint32 offset) override;

	virtual uint32 GetStride() const override { return mStride; };
	virtual uint32 GetCount() const override { return mCount; }
	virtual uint32 GetCapacity() const override { return mStride * mCount; };
private:
	uint32 mHandle = 0;
	uint32 mStride = 0;
	uint32 mCount = 0;
};

class FOpenGLVertexArray : public FVertexArray
{
public:
	FOpenGLVertexArray();
	virtual ~FOpenGLVertexArray();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void AddVertexBuffer(const std::shared_ptr<FVertexBuffer>& vertexBuffer) override;
	virtual void SetIndexBuffer(const std::shared_ptr<FIndexBuffer>& indexBuffer) override;

	virtual const std::vector<std::shared_ptr<FVertexBuffer>>& GetVertexBuffers() const { return mVertexBuffers; }
	virtual const std::shared_ptr<FIndexBuffer>& GetIndexBuffer() const { return mIndexBuffer; }
private:
	uint32 mHandle = 0;
	std::vector<std::shared_ptr<FVertexBuffer>> mVertexBuffers;
	std::shared_ptr<FIndexBuffer> mIndexBuffer;
};

}