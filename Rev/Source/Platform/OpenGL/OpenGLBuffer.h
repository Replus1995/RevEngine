#pragma once
#include "Rev/Render/RHI/RHIBuffer.h"

namespace Rev
{

class FOpenGLVertexBuffer : public FRHIVertexBuffer
{
public:
	FOpenGLVertexBuffer(uint32 size, const float* vertices = nullptr);
	virtual ~FOpenGLVertexBuffer();
	virtual const void* GetNativeHandle() const override { return &mHandle; }
	virtual void UpdateSubData(const void* data, uint32 size, uint32 offset) override;

private:
	uint32 mHandle = 0;
};

class FOpenGLIndexBuffer : public FRHIIndexBuffer
{
public:
	FOpenGLIndexBuffer(uint32 stride, uint32 count, const void* indices = nullptr);
	virtual ~FOpenGLIndexBuffer();
	virtual const void* GetNativeHandle() const override { return &mHandle; }
	virtual void UpdateSubData(const void* data, uint32 count, uint32 offset) override;

private:
	uint32 mHandle = 0;
};

class FOpenGLVertexArray : public FRHIVertexArray
{
public:
	FOpenGLVertexArray();
	virtual ~FOpenGLVertexArray();
	virtual const void* GetNativeHandle() const override { return &mHandle; }

	virtual void AddVertexBuffer(const Ref<FRHIVertexBuffer>& InVertexBuffer) override;
	virtual void SetIndexBuffer(const Ref<FRHIIndexBuffer>& InIndexBuffer) override;

	virtual const std::vector<Ref<FRHIVertexBuffer>>& GetVertexBuffers() const { return mVertexBuffers; }
	virtual const Ref<FRHIIndexBuffer>& GetIndexBuffer() const { return mIndexBuffer; }
private:
	uint32 mHandle = 0;
	std::vector<Ref<FRHIVertexBuffer>> mVertexBuffers;
	Ref<FRHIIndexBuffer> mIndexBuffer;
};

class FOpenGLUniformBuffer : public FRHIUniformBuffer
{
public:
	FOpenGLUniformBuffer(uint32 size);
	virtual ~FOpenGLUniformBuffer();
	virtual const void* GetNativeHandle() const override { return &mHandle; }
	virtual void UpdateSubData(const void* data, uint32 size, uint32 offset = 0) override;
private:
	uint32_t mHandle = 0;
};

}