#include "VkBuffer.h"
#include "VkCore.h"

namespace Rev
{

FVkBuffer::FVkBuffer()
{
}

FVkBuffer::FVkBuffer(FVkBuffer&& Other) noexcept
	: mBuffer(Other.mBuffer)
	, mAllocation(Other.mAllocation)
	, mAllocationInfo(Other.mAllocationInfo)
{
	Other.mBuffer = VK_NULL_HANDLE;
	Other.mAllocation = nullptr;
	Other.mAllocationInfo = {};
}

FVkBuffer::~FVkBuffer()
{
	if (mBuffer)
	{
		Release();
	}
}

FVkBuffer& FVkBuffer::operator=(FVkBuffer&& Other) noexcept
{
	mBuffer = Other.mBuffer;
	mAllocation = Other.mAllocation;
	mAllocationInfo = Other.mAllocationInfo;

	Other.mBuffer = VK_NULL_HANDLE;
	Other.mAllocation = nullptr;
	Other.mAllocationInfo = {};
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

FVkStageBuffer::FVkStageBuffer(uint64 Size, VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage)
{
	Allocate(Size, BufferUsage, MemoryUsage);
}

FVkStageBuffer::~FVkStageBuffer()
{
}

FVkVertexBuffer::FVkVertexBuffer(uint32 InSize, const float* InData)
	: FRHIVertexBuffer(InSize)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_GPU_ONLY);

	VkBufferDeviceAddressInfo DeviceAddressInfo{};
	DeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	DeviceAddressInfo.buffer = mBuffer;
	mDeviceAddress = vkGetBufferDeviceAddress(FVkCore::GetDevice(), &DeviceAddressInfo);
}

FVkVertexBuffer::~FVkVertexBuffer()
{
}

FVkIndexBuffer::FVkIndexBuffer(uint32 InStride, uint32 InCount, const void* InData)
	: FRHIIndexBuffer(InStride, InCount)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	Allocate(InStride * InCount, BufferUsage, VMA_MEMORY_USAGE_GPU_ONLY);
}

FVkIndexBuffer::~FVkIndexBuffer()
{
}

}