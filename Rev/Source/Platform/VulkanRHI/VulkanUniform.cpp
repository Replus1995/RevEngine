#include "VulkanUniform.h"
#include "VulkanCore.h"
#include "VulkanUtils.h"

namespace Rev
{


FVulkanUniformBuffer::FVulkanUniformBuffer(uint32 InSize, uint32 InBinding)
	: FRHIUniformBuffer(InSize)
	, mBinding(InBinding)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_CPU_TO_GPU);

	VkDescriptorSetLayoutBinding LayoutBinding{};
	LayoutBinding.binding = mBinding;
	LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LayoutBinding.descriptorCount = 1;
	LayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	LayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo LayoutCreateInfo{};
	LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutCreateInfo.bindingCount = 1;
	LayoutCreateInfo.pBindings = &LayoutBinding;

	REV_VK_CHECK_THROW(vkCreateDescriptorSetLayout(FVulkanCore::GetDevice(), &LayoutCreateInfo, nullptr, &mDescSetLayout), "failed to create descriptor set layout!");

	//mDescSet = FVulkanCore::GetContext()->GetDescriptorPool().Allocate(FVulkanCore::GetDevice(), mDescSetLayout);

	FVulkanCore::GetUniformManager()->AddBuffer(this);
}

FVulkanUniformBuffer::~FVulkanUniformBuffer()
{
	FVulkanCore::GetUniformManager()->RemoveBuffer(this);

	vkDestroyDescriptorSetLayout(FVulkanCore::GetDevice(), mDescSetLayout, nullptr);
}

void FVulkanUniformBuffer::UpdateSubData(const void* Data, uint32 Size, uint32 Offset)
{
	REV_CORE_ASSERT(Size + Offset <= mSize);
	uint8* DstMem = (uint8*)GetMappedData() + Offset;
	memcpy(DstMem, Data, Size);


	//VkDescriptorBufferInfo BufferInfo{};
	//BufferInfo.buffer = mBuffer;
	//BufferInfo.offset = (uint32_t)Offset;
	//BufferInfo.range = (uint32_t)Size;

	//VkWriteDescriptorSet DescriptorWrite{};
	//DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//DescriptorWrite.dstSet = mDescSet;
	//DescriptorWrite.dstBinding = mBinding;
	//DescriptorWrite.dstArrayElement = 0;
	//DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//DescriptorWrite.descriptorCount = 1;
	//DescriptorWrite.pBufferInfo = &BufferInfo;
	//DescriptorWrite.pImageInfo = nullptr; // Optional
	//DescriptorWrite.pTexelBufferView = nullptr; // Optional

	//vkUpdateDescriptorSets(FVulkanCore::GetDevice(), 1, &DescriptorWrite, 0, nullptr);

}

FVulkanUniformBufferDynamic::FVulkanUniformBufferDynamic(uint32 InSize, uint32 InBinding)
	: FRHIUniformBufferDynamic(InSize)
	, mBinding(InBinding)
{
	VkBufferUsageFlags BufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	Allocate(InSize, BufferUsage, VMA_MEMORY_USAGE_CPU_TO_GPU);

	VkDescriptorSetLayoutBinding LayoutBinding{};
	LayoutBinding.binding = mBinding;
	LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	LayoutBinding.descriptorCount = 1;
	LayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	LayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo LayoutCreateInfo{};
	LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutCreateInfo.bindingCount = 1;
	LayoutCreateInfo.pBindings = &LayoutBinding;

	REV_VK_CHECK_THROW(vkCreateDescriptorSetLayout(FVulkanCore::GetDevice(), &LayoutCreateInfo, nullptr, &mDescSetLayout), "failed to create descriptor set layout!");

	FVulkanCore::GetUniformManager()->AddBuffer(this);
}

FVulkanUniformBufferDynamic::~FVulkanUniformBufferDynamic()
{
	FVulkanCore::GetUniformManager()->RemoveBuffer(this);

	vkDestroyDescriptorSetLayout(FVulkanCore::GetDevice(), mDescSetLayout, nullptr);
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
	for (const FVulkanUniformBuffer* Buffer : mBufferVec)
	{
		Layouts.push_back(Buffer->GetLayout());
	}
	for (const FVulkanUniformBufferDynamic* Buffer : mBufferDynamicVec)
	{
		Layouts.push_back(Buffer->GetLayout());
	}
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