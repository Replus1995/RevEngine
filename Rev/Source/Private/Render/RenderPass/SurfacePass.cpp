#include "Rev/Render/RenderPass/SurfacePass.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FForwardSurfacePass::FForwardSurfacePass()
	: FRenderPass("ForwardSurfacePass")
{
}

FForwardSurfacePass::~FForwardSurfacePass()
{
}

void FForwardSurfacePass::RunPass()
{
	ClearRenderTarget();
}

FDeferredSurfacePass::FDeferredSurfacePass()
	: FRenderPass("SurfacePass")
{
}

FDeferredSurfacePass::~FDeferredSurfacePass()
{
}

}