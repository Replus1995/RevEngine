#include "VulkanBuffer.h"
#include "VulkanDynamicRHI.h"

namespace Rev
{

FVulkanBufferBase::FVulkanBufferBase()
{
}

FVulkanBufferBase::FVulkanBufferBase(FVulkanBufferBase&& Other) noexcept
	: Buffer(Other.Buffer)
	, mAllocation(Other.mAllocation)
	, mAllocationInfo(Other.mAllocationInfo)
{
	Other.Buffer = VK_NULL_HANDLE;
	Other.mAllocation = nullptr;
	Other.mAllocationInfo = {};
}

FVulkanBufferBase::~FVulkanBufferBase()
{
	if (Buffer)
	{
		Release();
	}
}

FVulkanBufferBase& FVulkanBufferBase::operator=(FVulkanBufferBase&& Other) noexcept
{
	Buffer = Other.Buffer;
	mAllocation = Other.mAllocation;
	mAllocationInfo = Other.mAllocationInfo;

	Other.Buffer = VK_NULL_HANDLE;
	Other.mAllocation = nullptr;
	Other.mAllocationInfo = {};

	return *this;
}

VkDeviceAddress FVulkanBufferBase::GetDeviceAddress() const
{
	/*
	//Need Flag: VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
	VkBufferDeviceAddressInfo DeviceAddressInfo{};
	DeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	DeviceAddressInfo.buffer = Buffer;
	return vkGetBufferDeviceAddress(FVulkanDynamicRHI::GetDevice(), &DeviceAddressInfo);
	*/
	return 0;
}

void FVulkanBufferBase::Allocate(uint64 Size, VkBufferUsageFlags BufferUsage, VmaAllocationCreateFlags CreateFlags)
{
	if (Buffer)
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
	AllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	AllocInfo.flags = CreateFlags;

	// allocate the buffer
	REV_VK_CHECK(vmaCreateBuffer(FVulkanDynamicRHI::GetAllocator(), &BufferCreateInfo, &AllocInfo, &Buffer, &mAllocation, &mAllocationInfo));
}

void FVulkanBufferBase::Release()
{
	vmaDestroyBuffer(FVulkanDynamicRHI::GetAllocator(), Buffer, mAllocation);
	Buffer = VK_NULL_HANDLE;
	mAllocation = nullptr;
	mAllocationInfo = {};
}

FVulkanStageBuffer::FVulkanStageBuffer(uint64 Size)
{
	Allocate(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
}

FVulkanStageBuffer::~FVulkanStageBuffer()
{
}

FVulkanBuffer::FVulkanBuffer(uint32 InSize, uint32 InStride, EBufferUsageFlags InUsage)
	: FRHIBuffer(InSize, InStride, InUsage)
{
	Allocate(InSize, TranslateUsageFlags(InUsage), GetAllocateFlags(InUsage));
}

FVulkanBuffer::~FVulkanBuffer()
{
}

VkIndexType FVulkanBuffer::GetIndexType() const
{
	switch (GetStride())
	{
	case 1:
		return VK_INDEX_TYPE_UINT8_KHR;
	case 2:
		return VK_INDEX_TYPE_UINT16;
	case 4:
		return VK_INDEX_TYPE_UINT32;
	default:
		break;
	}
	REV_CORE_ASSERT(false, "[FVulkanIndexBuffer] Unknown index buffer type");
	return VK_INDEX_TYPE_NONE_KHR;
}

VkBufferUsageFlags FVulkanBuffer::TranslateUsageFlags(EBufferUsageFlags InUsage)
{
	if (EnumHasAnyFlags(InUsage, EBufferUsageFlags::VertexBuffer))
	{
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
	if (EnumHasAnyFlags(InUsage, EBufferUsageFlags::IndexBuffer))
	{
		return  VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	return VkBufferUsageFlags();
}

VmaAllocationCreateFlags FVulkanBuffer::GetAllocateFlags(EBufferUsageFlags InUsage)
{
	VmaAllocationCreateFlags OutFlags = 0;
	if (EnumHasAnyFlags(InUsage, EBufferUsageFlags::KeepCPUAccess))
	{
		OutFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT; //TODO: Add UAV support
	}
	return OutFlags;
}

FVulkanUniformBuffer::FVulkanUniformBuffer(uint32 InSize)
	: FRHIUniformBuffer(InSize)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	Allocate(InSize, BufferUsage, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
}

FVulkanUniformBuffer::~FVulkanUniformBuffer()
{
}

void FVulkanUniformBuffer::UpdateSubData(const void* Data, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= mSize);

	void* pMappedData = nullptr;
	vmaMapMemory(FVulkanDynamicRHI::GetAllocator(), mAllocation, &pMappedData);
	if (pMappedData)
	{
		uint8* DstMem = (uint8*)pMappedData + Offset;
		memcpy(DstMem, Data, Size);
	}
	vmaUnmapMemory(FVulkanDynamicRHI::GetAllocator(), mAllocation);
}




}