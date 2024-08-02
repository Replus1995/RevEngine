#include "Rev/Render/RenderPipeline/SurfacePass.h"
#include "Rev/Render/RenderPipeline/RenderPipeline.h"
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
	FRenderPipeline::GetCurrentPipeline()->GetSceneProxy()->DrawScene();
}

FDeferredSurfacePass::FDeferredSurfacePass()
	: FRenderPass("SurfacePass")
{
}

FDeferredSurfacePass::~FDeferredSurfacePass()
{
}

}