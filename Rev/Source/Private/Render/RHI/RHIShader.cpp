#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

namespace Rev
{
Ref<FRHIShader>& FRHIGraphicsShaders::operator[](uint8 StageIndex)
{
	switch (StageIndex)
	{
	case SS_Vertex:
		return VertexShader;
	case SS_Hull:
		return HullShader;
	case SS_Domain:
		return DomainShader;
	case SS_Pixel:
		return PixelShader;
	case SS_Geometry:
		return GeometryShader;
	default:
		break;
	}
	REV_CORE_ASSERT(false, "GraphicsShaders index out of range");
	return VertexShader;
}

const Ref<FRHIShader>& FRHIGraphicsShaders::operator[](uint8 StageIndex) const
{
	switch (StageIndex)
	{
	case SS_Vertex:
		return VertexShader;
	case SS_Hull:
		return HullShader;
	case SS_Domain:
		return DomainShader;
	case SS_Pixel:
		return PixelShader;
	case SS_Geometry:
		return GeometryShader;
	default:
		break;
	}
	REV_CORE_ASSERT(false, "GraphicsShaders index out of range");
	return VertexShader;
}

}