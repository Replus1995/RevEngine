#pragma once
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{

class REV_API SurfaceMaterial : public Material
{
public:
	SurfaceMaterial();
	virtual ~SurfaceMaterial();

	virtual void Compile() override;
	virtual void PreDraw() override;
	virtual void PostDraw() override;

public:
	EMaterialBlendMode BlendMode = MBM_Opaque;
	EMaterialShadingModel ShadingModel = MSM_Default;
	bool TwoSided = false;
	float MaskClip = 0.5f;
};

}