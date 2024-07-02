#pragma once
#include <memory>

namespace Pine
{

enum class ERenderAPI : uint8_t
{
    None = 0,
    OpenGL = 1,
    Vulkan = 2
};
ERenderAPI GetRenderAPI();

enum class EShaderDataType
{
	None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
};
uint32_t ShaderDataTypeSize(EShaderDataType type);

}