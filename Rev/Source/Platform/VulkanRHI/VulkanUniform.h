#pragma once
#include "VulkanBuffer.h"
#include <set>

namespace Rev
{

class FVulkanUniformBuffer : public FRHIUniformBuffer, FVulkanBuffer
{
public:
	FVulkanUniformBuffer(uint32 InSize, uint32 InBinding);
	virtual ~FVulkanUniformBuffer();
	virtual const void* GetNativeHandle() const override { return mBuffer; }
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset) override;
	//VkDescriptorSetLayout GetLayout() const { return mDescSetLayout; }

protected:
	uint32 mBinding = 0;
	//VkDescriptorSetLayout mDescSetLayout = VK_NULL_HANDLE;
};


class FVulkanUniformBufferDynamic : public FRHIUniformBufferDynamic, FVulkanBuffer
{
public:
	FVulkanUniformBufferDynamic(uint32 InSize, uint32 InBinding);
	virtual ~FVulkanUniformBufferDynamic();
	virtual const void* GetNativeHandle() const override { return mBuffer; }
	virtual uint32 UpdateSubData(const void* Data, uint32 Size) override;
	virtual void Clear() override;
	//VkDescriptorSetLayout GetLayout() const { return mDescSetLayout; }

protected:
	uint32 mDynamicOffset = 0;
	uint32 mBinding = 0;
	//VkDescriptorSetLayout mDescSetLayout = VK_NULL_HANDLE;
};

class FVulkanUniformManager
{
public:
	FVulkanUniformManager();
	~FVulkanUniformManager();

	void AddBuffer(FVulkanUniformBuffer* InBuffer);
	void AddBuffer(FVulkanUniformBufferDynamic* InBuffer);
	void RemoveBuffer(FVulkanUniformBuffer* InBuffer);
	void RemoveBuffer(FVulkanUniformBufferDynamic* InBuffer);
	std::vector<VkDescriptorSetLayout> GetLayouts() const;

	void PrepareDraw();

private:
	std::set<FVulkanUniformBuffer*> mBufferVec;
	std::set<FVulkanUniformBufferDynamic*> mBufferDynamicVec;
};

}