#include "VulkanVertexInputDesc.h"

namespace Rev
{

namespace
{

FVkVertexInputDesc MakeDesc_Surface()
{
	FVkVertexInputDesc Desc;

	//Position
	Desc.Bindings[0] = {};
	Desc.Bindings[0].binding = 0;
	Desc.Bindings[0].stride = sizeof(float) * 3;
	Desc.Bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	Desc.Attributes[0] = {};
	Desc.Attributes[0].binding = 0;
	Desc.Attributes[0].location = 0;
	Desc.Attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	Desc.Attributes[0].offset = 0;

	//Normal
	Desc.Bindings[1] = {};
	Desc.Bindings[1].binding = 1;
	Desc.Bindings[1].stride = sizeof(float) * 3;
	Desc.Bindings[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	Desc.Attributes[1] = {};
	Desc.Attributes[1].binding = 1;
	Desc.Attributes[1].location = 1;
	Desc.Attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	Desc.Attributes[0].offset = 0;

	//Tangent
	Desc.Bindings[2] = {};
	Desc.Bindings[2].binding = 2;
	Desc.Bindings[2].stride = sizeof(float) * 4;
	Desc.Bindings[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	Desc.Attributes[2] = {};
	Desc.Attributes[2].binding = 2;
	Desc.Attributes[2].location = 2;
	Desc.Attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	Desc.Attributes[2].offset = 0;

	//TexCoord
	Desc.Bindings[3] = {};
	Desc.Bindings[3].binding = 2;
	Desc.Bindings[3].stride = sizeof(float) * 2;
	Desc.Bindings[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	Desc.Attributes[3] = {};
	Desc.Attributes[3].binding = 3;
	Desc.Attributes[3].location = 3;
	Desc.Attributes[3].format = VK_FORMAT_R32G32_SFLOAT;
	Desc.Attributes[3].offset = 0;

	//Count
	Desc.BindingsCount = 4;
	Desc.AttributesCount = 4;
}

}

FVkVertexInputDesc GetVkVertexInputDesc(EMaterialDomain InDomain)
{
	return FVkVertexInputDesc();
}

}