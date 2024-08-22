#include "VkShader.h"
#include "Rev/Core/Assert.h"

#include "VkCore.h"

namespace Rev
{

FVkShader::FVkShader(ERHIShaderStage InStage, const FBuffer& InCompiledData)
	: FRHIShader(InStage)
{
	VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
	ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderModuleCreateInfo.codeSize = InCompiledData.Size();
	ShaderModuleCreateInfo.pCode = InCompiledData.DataAs<uint32_t>();

	REV_VK_CHECK_THROW(vkCreateShaderModule(FVkCore::GetDevice(), &ShaderModuleCreateInfo, nullptr, &mModule), "[FVkShader] Failed to create shader module!");
}

FVkShader::~FVkShader()
{
	vkDestroyShaderModule(FVkCore::GetDevice(), mModule, nullptr);
}

VkShaderStageFlagBits FVkShader::TranslateShaderStage(ERHIShaderStage InStage)
{
	switch (InStage)
	{
	case ERHIShaderStage::Vertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case ERHIShaderStage::Fragment:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case ERHIShaderStage::TessControl:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case ERHIShaderStage::TessEval:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case ERHIShaderStage::Geometry:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	case ERHIShaderStage::Compute:
		return VK_SHADER_STAGE_COMPUTE_BIT;
	default:
		break;
	}
	REV_CORE_ASSERT(false);
	return VkShaderStageFlagBits(0);
}

}


