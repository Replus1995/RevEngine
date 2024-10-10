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


	FVulkanCore::GetUniformManager()->AddBuffer(this);
}

FVulkanUniformBuffer::~FVulkanUniformBuffer()
{
	FVulkanCore::GetUniformManager()->RemoveBuffer(this);
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


	FVulkanCore::GetUniformManager()->AddBuffer(this);
}

FVulkanUniformBufferDynamic::~FVulkanUniformBufferDynamic()
{
	FVulkanCore::GetUniformManager()->RemoveBuffer(this);
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

FVulkanUniformManager::FVulkanUniformManager()
{
}

FVulkanUniformManager::~FVulkanUniformManager()
{
	REV_CORE_ASSERT(mBufferVec.empty());
	REV_CORE_ASSERT(mBufferDynamicVec.empty());
}

void FVulkanUniformManager::AddBuffer(FVulkanUniformBuffer* InBuffer)
{
	mBufferVec.insert(InBuffer);
}

void FVulkanUniformManager::AddBuffer(FVulkanUniformBufferDynamic* InBuffer)
{
	mBufferDynamicVec.insert(InBuffer);
}

void FVulkanUniformManager::RemoveBuffer(FVulkanUniformBuffer* InBuffer)
{
	mBufferVec.erase(InBuffer);
}

void FVulkanUniformManager::RemoveBuffer(FVulkanUniformBufferDynamic* InBuffer)
{
	mBufferDynamicVec.erase(InBuffer);
}

std::vector<VkDescriptorSetLayout> FVulkanUniformManager::GetLayouts() const
{
	std::vector<VkDescriptorSetLayout> Layouts;
	/*for (const FVulkanUniformBuffer* Buffer : mBufferVec)
	{
		Layouts.push_back(Buffer->GetLayout());
	}
	for (const FVulkanUniformBufferDynamic* Buffer : mBufferDynamicVec)
	{
		Layouts.push_back(Buffer->GetLayout());
	}*/
	return Layouts;
}

void FVulkanUniformManager::PrepareDraw()
{
	for (FVulkanUniformBufferDynamic* Buffer : mBufferDynamicVec)
	{
		Buffer->Clear();
	}
}

}