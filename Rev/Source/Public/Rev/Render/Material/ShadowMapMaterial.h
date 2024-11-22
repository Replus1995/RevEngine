#pragma once
#include "Rev/Render/Material/Material.h"

namespace Rev
{

class REV_API FShadowMapMaterial : public FMaterial
{
public:
	FShadowMapMaterial();
	virtual ~FShadowMapMaterial();

	virtual void Compile() override;
};

}