#include "VulkanBuffer.h"
#include "VulkanCore.h"
#include "VulkanUtils.h"

namespace Rev
{

FVulkanBuffer::FVulkanBuffer()
{
}

FVulkanBuffer::FVulkanBuffer(FVulkanBuffer&& Other) noexcept
	: mBuffer(Other.mBuffer)
	, mAllocation(Other.mAllocation)
	, mAllocationInfo(Other.mAllocationInfo)
{
	Other.mBuffer = VK_NULL_HANDLE;
	Other.mAllocation = nullptr;
	Other.mAllocationInfo = {};
}

FVulkanBuffer::~FVulkanBuffer()
{
	if (mBuffer)
	{
		Release();
	}
}

FVulkanBuffer& FVulkanBuffer::operator=(FVulkanBuffer&& Other) noexcept
{
	mBuffer = Other.mBuffer;
	mAllocation = Other.mAllocation;
	mAllocationInfo = Other.mAllocationInfo;

	Other.mBuffer = VK_NULL_HANDLE;
	Other.mAllocation = nullptr;
	Other.mAllocationInfo = {};

	return *this;
}

void FVulkanBuffer::Allocate(uint64 Size, VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage)
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
	REV_VK_CHECK(vmaCreateBuffer(FVulkanCore::GetAllocator(), &BufferCreateInfo, &AllocInfo, &mBuffer, &mAllocation, &mAllocationInfo));
}

void FVulkanBuffer::Release()
{
	vmaDestroyBuffer(FVulkanCore::GetAllocator(), mBuffer, mAllocation);
	mBuffer = VK_NULL_HANDLE;
	mAllocation = nullptr;
	mAllocationInfo = {};
}

FVulkanStageBuffer::FVulkanStageBuffer(uint64 Size, VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage)
{
	Allocate(Size, BufferUsage, MemoryUsage);
}

FVulkanStageBuffer::~FVulkanStageBuffer()
{
}

FVulkanVertexBuffer::FVulkanVertexBuffer(uint32 InSize, const float* InData)
	: FRHIVertexBuffer(InSize)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_GPU_ONLY);

	VkBufferDeviceAddressInfo DeviceAddressInfo{};
	DeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	DeviceAddressInfo.buffer = mBuffer;
	mDeviceAddress = vkGetBufferDeviceAddress(FVulkanCore::GetDevice(), &DeviceAddressInfo);
}

FVulkanVertexBuffer::~FVulkanVertexBuffer()
{
}

void FVulkanVertexBuffer::UpdateSubData(const void* Data, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= mSize);
	FVulkanUtils::ImmediateUploadBuffer(mBuffer, Data, Size, Offset);
}

FVulkanIndexBuffer::FVulkanIndexBuffer(uint32 InStride, uint32 InCount, const void* InData)
	: FRHIIndexBuffer(InStride, InCount)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	Allocate(InStride * InCount, BufferUsage, VMA_MEMORY_USAGE_GPU_ONLY);
}

FVulkanIndexBuffer::~FVulkanIndexBuffer()
{
}

void FVulkanIndexBuffer::UpdateSubData(const void* Data, uint32 Count, uint32 Offset)
{
	REV_CORE_ASSERT(Count + Offset <= mCount);
	FVulkanUtils::ImmediateUploadBuffer(mBuffer, Data, Count * mStride, Offset * mStride);
}

FVulkanUniformBuffer::FVulkanUniformBuffer(uint32 InSize)
	: FRHIUniformBuffer(InSize)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_CPU_ONLY);
}

FVulkanUniformBuffer::~FVulkanUniformBuffer()
{

}

void FVulkanUniformBuffer::UpdateSubData(const void* Data, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= mSize);
	uint8* DstMem = (uint8*)GetMappedData() + Offset;
	memcpy(DstMem, Data, Size);
}

}