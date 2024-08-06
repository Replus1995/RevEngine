#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/FullScreenPass.h"

namespace Rev
{
class Material;
class FSkyboxPass : public FFullScreenPass
{
public:
	FSkyboxPass();
	virtual ~FSkyboxPass();
	virtual void BeginPass() override;
	virtual void RunPass() override;

protected:
	Ref<Material> mMaterial;
};

}