#include "pinepch.h"
#include "Pine/Render/RenderCore.h"
#include "Pine/Core/Base.h"

namespace Pine
{

uint32_t ShaderDataTypeSize(EShaderDataType type)
{
	switch (type)
	{
	case EShaderDataType::Float:    return 4;
	case EShaderDataType::Float2:   return 4 * 2;
	case EShaderDataType::Float3:   return 4 * 3;
	case EShaderDataType::Float4:   return 4 * 4;
	case EShaderDataType::Mat3:     return 4 * 3 * 3;
	case EShaderDataType::Mat4:     return 4 * 4 * 4;
	case EShaderDataType::Int:      return 4;
	case EShaderDataType::Int2:     return 4 * 2;
	case EShaderDataType::Int3:     return 4 * 3;
	case EShaderDataType::Int4:     return 4 * 4;
	case EShaderDataType::Bool:     return 1;
	}

	PE_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

}