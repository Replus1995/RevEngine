#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{

struct FVulkanDescriptorLayoutBuilder
{
public:
	std::vector<VkDescriptorSetLayoutBinding> Bindings;

    void Add(uint32 InBinding, VkDescriptorType InType);
    void Clear();
    VkDescriptorSetLayout Build(VkDevice InDevice, VkShaderStageFlags InShaderStages, VkDescriptorSetLayoutCreateFlags InFlags = 0, void* pNext = nullptr);

};

struct FVulkanDescriptorPool
{
public:
    VkDescriptorPool Pool;

    void CreatePool(VkDevice InDevice, const std::vector<VkDescriptorPoolSize>& InPoolSizes);
    void ResetPool(VkDevice InDevice); //clear all descriptor sets
    void Cleanup(VkDevice InDevice);
    VkDescriptorSet Allocate(VkDevice InDevice, VkDescriptorSetLayout InLayout);
};

}