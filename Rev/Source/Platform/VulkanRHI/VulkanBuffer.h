#pragma once
#include "Rev/Render/RHI/RHIBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Rev
{

class FVulkanBuffer
{
public:
    FVulkanBuffer();
	FVulkanBuffer(const FVulkanBuffer& Other) = delete;
	FVulkanBuffer(FVulkanBuffer&& Other) noexcept;
	virtual ~FVulkanBuffer();

	FVulkanBuffer& operator=(const FVulkanBuffer& Other) = delete;
	FVulkanBuffer& operator=(FVulkanBuffer&& Other) noexcept;

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

class FVulkanStageBuffer : public FVulkanBuffer
{
public:
	FVulkanStageBuffer(uint64 Size, VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage);
	virtual ~FVulkanStageBuffer();
};

class FVulkanVertexBuffer : public FRHIVertexBuffer, FVulkanBuffer
{
public:
	FVulkanVertexBuffer(uint32 InSize, bool bDynamic);
	virtual ~FVulkanVertexBuffer();
	virtual const void* GetNativeHandle() const override { return mBuffer; }

private:
	VkDeviceAddress mDeviceAddress = 0;
};

class FVulkanIndexBuffer : public FRHIIndexBuffer, FVulkanBuffer
{
public:
	FVulkanIndexBuffer(uint32 InStride, uint32 InCount, bool bDynamic);
	virtual ~FVulkanIndexBuffer();
	virtual const void* GetNativeHandle() const override { return mBuffer; }

	VkIndexType GetIndexType() const { return mIndexType; }

private:
	VkIndexType mIndexType = VK_INDEX_TYPE_NONE_KHR;
};

}