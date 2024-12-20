#pragma once
#include "Rev/Render/RHI/RHIBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Rev
{

class FVulkanBufferBase
{
public:
    FVulkanBufferBase();
	FVulkanBufferBase(const FVulkanBufferBase& Other) = delete;
	FVulkanBufferBase(FVulkanBufferBase&& Other) noexcept;
	virtual ~FVulkanBufferBase();

	FVulkanBufferBase& operator=(const FVulkanBufferBase& Other) = delete;
	FVulkanBufferBase& operator=(FVulkanBufferBase&& Other) noexcept;

	VkDeviceAddress GetDeviceAddress() const;
	VkBuffer Buffer = VK_NULL_HANDLE;

protected:
    void Allocate(uint64 Size, VkBufferUsageFlags BufferUsage, VmaAllocationCreateFlags CreateFlags);
    void Release();

    VmaAllocation mAllocation = nullptr;
    VmaAllocationInfo mAllocationInfo = {};
};

class FVulkanStageBuffer : public FVulkanBufferBase
{
public:
	FVulkanStageBuffer(uint64 Size);
	virtual ~FVulkanStageBuffer();
	void* GetMappedData() const { return mAllocationInfo.pMappedData; }
};


class FVulkanBuffer : public FRHIBuffer, FVulkanBufferBase
{
public:
	FVulkanBuffer(const FRHIBufferDesc& InDesc);
	virtual ~FVulkanBuffer();
	virtual const void* GetNativeHandle() const override { return Buffer; }

	VkIndexType GetIndexType() const;
protected:
	static VkBufferUsageFlags TranslateUsageFlags(EBufferUsageFlags InUsage);
	static VmaAllocationCreateFlags GetAllocateFlags(EBufferUsageFlags InUsage);
};

class FVulkanUniformBuffer : public FRHIUniformBuffer, FVulkanBufferBase
{
public:
	FVulkanUniformBuffer(uint32 InSize);
	virtual ~FVulkanUniformBuffer();
	virtual const void* GetNativeHandle() const override { return Buffer; }
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset) override;
};

}