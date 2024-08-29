#include "VkBuffer.h"
#include "VkCore.h"

namespace Rev
{

FVkBuffer::FVkBuffer()
{
}

FVkBuffer::~FVkBuffer()
{
	/*if (mBuffer)
	{
		Release();
	}*/
}

void FVkBuffer::Allocate(uint64 Size, VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage)
{
	if (mBuffer)
	{
		Release();
	}

	// allocate buffer
	VkBufferCreateInfo BufferCreateInfo{};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = nullptr;
	BufferCreateInfo.size = Size;
	BufferCreateInfo.usage = BufferUsage;

	VmaAllocationCreateInfo AllocInfo = {};
	AllocInfo.usage = MemoryUsage;
	AllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	// allocate the buffer
	REV_VK_CHECK(vmaCreateBuffer(FVkCore::GetAllocator(), &BufferCreateInfo, &AllocInfo, &mBuffer, &mAllocation, &mAllocationInfo));
}

void FVkBuffer::Release()
{
	vmaDestroyBuffer(FVkCore::GetAllocator(), mBuffer, mAllocation);
	mBuffer = VK_NULL_HANDLE;
	mAllocation = nullptr;
	mAllocationInfo = {};
}

}