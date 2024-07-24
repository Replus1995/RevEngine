#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Rev
{

const Ref<FRHIShader>& FRHIGraphicsShaders::operator[](ERHIShaderStage Stage) const
{
	switch (Stage)
	{
	case Rev::ERHIShaderStage::Vertex:
		return VertexShader;
	case Rev::ERHIShaderStage::TessControl:
		return HullShader;
	case Rev::ERHIShaderStage::TessEval:
		return DomainShader;
	case Rev::ERHIShaderStage::Fragment:
		return PixelShader;
	case Rev::ERHIShaderStage::Geometry:
		return GeometryShader;
	default:
		break;
	}
	RE_CORE_ASSERT(false, "GraphicsShaders index out of range");
	return VertexShader;
}

}