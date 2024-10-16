#include "VulkanBuffer.h"
#include "VulkanDynamicRHI.h"

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
	REV_VK_CHECK(vmaCreateBuffer(FVulkanDynamicRHI::GetAllocator(), &BufferCreateInfo, &AllocInfo, &mBuffer, &mAllocation, &mAllocationInfo));
}

void FVulkanBuffer::Release()
{
	vmaDestroyBuffer(FVulkanDynamicRHI::GetAllocator(), mBuffer, mAllocation);
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

FVulkanVertexBuffer::FVulkanVertexBuffer(uint32 InSize, bool bDynamic)
	: FRHIVertexBuffer(InSize)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_GPU_ONLY);

	VkBufferDeviceAddressInfo DeviceAddressInfo{};
	DeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	DeviceAddressInfo.buffer = mBuffer;
	mDeviceAddress = vkGetBufferDeviceAddress(FVulkanDynamicRHI::GetDevice(), &DeviceAddressInfo);
}

FVulkanVertexBuffer::~FVulkanVertexBuffer()
{
}

FVulkanIndexBuffer::FVulkanIndexBuffer(uint32 InStride, uint32 InCount, bool bDynamic)
	: FRHIIndexBuffer(InStride, InCount)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	Allocate(GetCapacity(), BufferUsage, VMA_MEMORY_USAGE_GPU_ONLY);

	switch (InStride)
	{
	case 1:
		mIndexType = VK_INDEX_TYPE_UINT8_KHR;
		break;
	case 2:
		mIndexType = VK_INDEX_TYPE_UINT16;
		break;
	case 4:
		mIndexType = VK_INDEX_TYPE_UINT32;
		break;
	default:
		REV_CORE_ASSERT(false, "[FVulkanIndexBuffer] Unknown index buffer type");
		break;
	}
}

FVulkanIndexBuffer::~FVulkanIndexBuffer()
{
}

}