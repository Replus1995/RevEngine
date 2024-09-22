#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{

enum EPipelineBindPoint : uint8
{
	PBP_Graphics,
	PBP_Compute,
	//PBP_RayTracing,
	//PBP_Mesh
};

struct FPipelineDesc
{
public:
	EPipelineBindPoint BindPoint = PBP_Graphics;
	EDrawMode DrawMode;
	EBlendMode BlendMode;
	ECullFaceMode CullFaceMode;
	EDepthTestMode DepthTestMode;
	
};

class FRHIPipeline
{
public:
	FRHIPipeline();
	virtual ~FRHIPipeline();
};

}