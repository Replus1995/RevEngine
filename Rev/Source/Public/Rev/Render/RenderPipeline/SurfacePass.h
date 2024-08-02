#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/RenderPass.h"

namespace Rev
{

class SceneRenderProxy;
class FForwardSurfacePass : public FRenderPass
{
public:
	FForwardSurfacePass();
	virtual ~FForwardSurfacePass();
	virtual void RunPass() override;
};


class FDeferredSurfacePass : public FRenderPass
{
public:
	FDeferredSurfacePass();
	virtual ~FDeferredSurfacePass();
};

}