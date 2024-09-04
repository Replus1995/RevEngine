#include "VulkanDescriptor.h"
#include "VulkanDefines.h"

namespace Rev::VkUtils
{

void FVulkanDescriptorLayoutBuilder::Add(uint32 InBinding, VkDescriptorType InType)
{
    VkDescriptorSetLayoutBinding NewBinding{};
    NewBinding.binding = InBinding;
    NewBinding.descriptorCount = 1;
    NewBinding.descriptorType = InType;

    Bindings.push_back(NewBinding);
}

void FVulkanDescriptorLayoutBuilder::Clear()
{
    Bindings.clear();
}

VkDescriptorSetLayout FVulkanDescriptorLayoutBuilder::Build(VkDevice InDevice, VkShaderStageFlags InShaderStages, VkDescriptorSetLayoutCreateFlags InFlags, void* pNext)
{
    for (auto& Binding : Bindings) {
        Binding.stageFlags |= InShaderStages;
    }

    VkDescriptorSetLayoutCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    CreateInfo.pBindings = Bindings.data();
    CreateInfo.bindingCount = (uint32)Bindings.size();
    CreateInfo.flags = InFlags;
    CreateInfo.pNext = pNext;

    VkDescriptorSetLayout set;
    REV_VK_CHECK(vkCreateDescriptorSetLayout(InDevice, &CreateInfo, nullptr, &set));

    return set;
}

void FVulkanDescriptorAllocator::Init(VkDevice InDevice, const std::vector<VkDescriptorPoolSize>& InPoolSizes)
{
    uint32 MaxSets = 0;
    for (auto& PoolSize : InPoolSizes)
    {
        MaxSets += PoolSize.descriptorCount;
    }

    VkDescriptorPoolCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    CreateInfo.flags = 0;
    CreateInfo.maxSets = MaxSets;
    CreateInfo.poolSizeCount = (uint32)InPoolSizes.size();
    CreateInfo.pPoolSizes = InPoolSizes.data();

    vkCreateDescriptorPool(InDevice, &CreateInfo, nullptr, &Pool);
}

void FVulkanDescriptorAllocator::Reset(VkDevice InDevice)
{
    vkResetDescriptorPool(InDevice, Pool, 0);
}

void FVulkanDescriptorAllocator::Cleanup(VkDevice InDevice)
{
    vkDestroyDescriptorPool(InDevice, Pool, nullptr);
}

VkDescriptorSet FVulkanDescriptorAllocator::Allocate(VkDevice InDevice, VkDescriptorSetLayout InLayout)
{
    VkDescriptorSetAllocateInfo AllocateInfo{};
    AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocateInfo.pNext = nullptr;
    AllocateInfo.descriptorPool = Pool;
    AllocateInfo.descriptorSetCount = 1;
    AllocateInfo.pSetLayouts = &InLayout;

    VkDescriptorSet OutSet;
    REV_VK_CHECK(vkAllocateDescriptorSets(InDevice, &AllocateInfo, &OutSet));

    return OutSet;
}

}


