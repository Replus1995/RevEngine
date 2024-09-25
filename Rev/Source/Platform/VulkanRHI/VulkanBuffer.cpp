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

FVulkanUniformBuffer::FVulkanUniformBuffer(uint32 InSize, uint32 InBinding)
	: FRHIUniformBuffer(InSize)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_CPU_ONLY);

	VkDescriptorSetLayoutBinding LayoutBinding{};
	LayoutBinding.binding = InBinding;
	LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LayoutBinding.descriptorCount = 1;
	LayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	LayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo LayoutCreateInfo{};
	LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutCreateInfo.bindingCount = 1;
	LayoutCreateInfo.pBindings = &LayoutBinding;

	REV_VK_CHECK_THROW(vkCreateDescriptorSetLayout(FVulkanCore::GetDevice(), &LayoutCreateInfo, nullptr, &mDescSetLayout), "failed to create descriptor set layout!");

	mDescSet = FVulkanCore::GetContext()->GetDescriptorPool().Allocate(FVulkanCore::GetDevice(), mDescSetLayout);

}

FVulkanUniformBuffer::~FVulkanUniformBuffer()
{
	vkDestroyDescriptorSetLayout(FVulkanCore::GetDevice(), mDescSetLayout, nullptr);
}

void FVulkanUniformBuffer::UpdateSubData(const void* Data, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= mSize);
	uint8* DstMem = (uint8*)GetMappedData() + Offset;
	memcpy(DstMem, Data, Size);


	VkDescriptorBufferInfo BufferInfo{};
	BufferInfo.buffer = mBuffer;
	BufferInfo.offset = (uint32_t)Offset;
	BufferInfo.range = (uint32_t)Size;

	VkWriteDescriptorSet DescriptorWrite{};
	DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	DescriptorWrite.dstSet = mDescSet;
	DescriptorWrite.dstBinding = 0;
	DescriptorWrite.dstArrayElement = 0;
	DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorWrite.descriptorCount = 1;
	DescriptorWrite.pBufferInfo = &BufferInfo;
	DescriptorWrite.pImageInfo = nullptr; // Optional
	DescriptorWrite.pTexelBufferView = nullptr; // Optional

	vkUpdateDescriptorSets(FVulkanCore::GetDevice(), 1, &DescriptorWrite, 0, nullptr);
}

}