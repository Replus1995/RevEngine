#include "Rev/Render/RenderPass/SurfacePass.h"

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