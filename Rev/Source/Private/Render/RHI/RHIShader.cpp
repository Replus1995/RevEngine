#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

namespace Rev
{
Ref<FRHIShader>& FRHIGraphicsShaders::operator[](EShaderStage Stage)
{
	switch (Stage)
	{
	case EShaderStage::Vertex:
		return VertexShader;
	case EShaderStage::Hull:
		return HullShader;
	case EShaderStage::Domain:
		return DomainShader;
	case EShaderStage::Pixel:
		return PixelShader;
	case EShaderStage::Geometry:
		return GeometryShader;
	default:
		break;
	}
	REV_CORE_ASSERT(false, "GraphicsShaders index out of range");
	return VertexShader;
}

const Ref<FRHIShader>& FRHIGraphicsShaders::operator[](EShaderStage Stage) const
{
	switch (Stage)
	{
	case EShaderStage::Vertex:
		return VertexShader;
	case EShaderStage::Hull:
		return HullShader;
	case EShaderStage::Domain:
		return DomainShader;
	case EShaderStage::Pixel:
		return PixelShader;
	case EShaderStage::Geometry:
		return GeometryShader;
	default:
		break;
	}
	REV_CORE_ASSERT(false, "GraphicsShaders index out of range");
	return VertexShader;
}

}