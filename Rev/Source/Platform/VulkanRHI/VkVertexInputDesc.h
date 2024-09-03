#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"
#include <vulkan/vulkan.h>

#define REV_VK_MAX_VERTEX_BINDINGS 8
#define REV_VK_MAX_VERTEX_ATTRIBUTES 8

namespace Rev
{

struct FVkVertexInputDesc
{
	VkVertexInputBindingDescription Bindings[REV_VK_MAX_VERTEX_BINDINGS];
	uint32 BindingsCount = 0;
	VkVertexInputAttributeDescription Attributes[REV_VK_MAX_VERTEX_ATTRIBUTES];
	uint32 AttributesCount = 0;
};

FVkVertexInputDesc GetVkVertexInputDesc(EMaterialDomain InDomain);

}