#pragma once
#include "Rev/Render/RHI/RHIBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Rev
{

class FVkBuffer
{
public:
    FVkBuffer();
	FVkBuffer(const FVkBuffer& Other) = delete;
	FVkBuffer(FVkBuffer&& Other) noexcept;
	virtual ~FVkBuffer();

	FVkBuffer& operator=(const FVkBuffer& Other) = delete;
	FVkBuffer& operator=(FVkBuffer&& Other) noexcept;

	VkBuffer GetBuffer() const { return mBuffer; }
	void* GetMappedData() const { return mAllocationInfo.pMappedData; }

protected:
    void Allocate(uint64 Size, VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage);
    void Release();

protected:
    VkBuffer mBuffer = VK_NULL_HANDLE;
    VmaAllocation mAllocation = nullptr;
    VmaAllocationInfo mAllocationInfo = {};
};

class FVkStageBuffer : public FVkBuffer
{
public:
	FVkStageBuffer(uint64 Size, VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage);
	virtual ~FVkStageBuffer();
};

class FVkVertexBuffer : public FRHIVertexBuffer, FVkBuffer
{
public:
	FVkVertexBuffer(uint32 InSize, const float* InData = nullptr);
	virtual ~FVkVertexBuffer();
	virtual const void* GetNativeHandle() const override { return mBuffer; }
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset) override;
private:
	VkDeviceAddress mDeviceAddress = 0;
};

class FVkIndexBuffer : public FRHIIndexBuffer, FVkBuffer
{
public:
	FVkIndexBuffer(uint32 InStride, uint32 InCount, const void* InData = nullptr);
	virtual ~FVkIndexBuffer();
	virtual const void* GetNativeHandle() const override { return mBuffer; }
	virtual void UpdateSubData(const void* Data, uint32 Count, uint32 Offset) override;

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