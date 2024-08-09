#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPass/RenderPass.h"

namespace Rev
{

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