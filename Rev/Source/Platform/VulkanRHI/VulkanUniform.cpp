#include "VulkanUniform.h"
#include "VulkanCore.h"
#include "VulkanUtils.h"

namespace Rev
{


FVulkanUniformBuffer::FVulkanUniformBuffer(uint32 InSize)
	: FRHIUniformBuffer(InSize)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_CPU_TO_GPU);
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

FVulkanUniformBufferDynamic::FVulkanUniformBufferDynamic(uint32 InSize)
	: FRHIUniformBufferDynamic(InSize)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_CPU_TO_GPU);
}

FVulkanUniformBufferDynamic::~FVulkanUniformBufferDynamic()
{
}

uint32 FVulkanUniformBufferDynamic::UpdateSubData(const void* Data, uint32 Size)
{
	REV_CORE_ASSERT(mDynamicOffset + Size <= mSize);
	uint8* DstMem = (uint8*)GetMappedData() + mDynamicOffset;
	memcpy(DstMem, Data, Size);


	mDynamicOffset += Size;
	return mDynamicOffset;
}

void FVulkanUniformBufferDynamic::Clear()
{
	mDynamicOffset = 0;
}

}