#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{


FRenderer::FRenderer(FSceneProxy* InSceneProxy)
	: mSceneProxy(InSceneProxy)
{
}

FRenderer::~FRenderer()
{
}

void FRenderer::BeginFrame()
{
}

void FRenderer::DrawFrame()
{
}

void FRenderer::EndFrame()
{
}

}