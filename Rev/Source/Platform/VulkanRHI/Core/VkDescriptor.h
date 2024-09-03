#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev::VkUtils
{

struct FVkDescriptorLayoutBuilder
{
public:
	std::vector<VkDescriptorSetLayoutBinding> Bindings;

    void Add(uint32 InBinding, VkDescriptorType InType);
    void Clear();
    VkDescriptorSetLayout Build(VkDevice InDevice, VkShaderStageFlags InShaderStages, VkDescriptorSetLayoutCreateFlags InFlags = 0, void* pNext = nullptr);

};

struct FVkDescriptorAllocator
{
public:
    VkDescriptorPool Pool;

    void Init(VkDevice InDevice, const std::vector<VkDescriptorPoolSize>& InPoolSizes);
    void Reset(VkDevice InDevice); //clear all descriptor sets
    void Cleanup(VkDevice InDevice);
    VkDescriptorSet Allocate(VkDevice InDevice, VkDescriptorSetLayout InLayout);
};

}