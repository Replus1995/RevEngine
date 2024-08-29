#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Rev
{

class FVkBuffer
{
public:
    FVkBuffer();
    ~FVkBuffer();
    void Allocate(uint64 Size, VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage);
    void Release();

    VkBuffer GetBuffer() const { return mBuffer; }
    void* GetMappedData() const { return mAllocationInfo.pMappedData; }
private:
    VkBuffer mBuffer = VK_NULL_HANDLE;
    VmaAllocation mAllocation = nullptr;
    VmaAllocationInfo mAllocationInfo = {};
};


}