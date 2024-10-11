#pragma once
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/RHI/RHIBuffer.h"

namespace Rev
{
class FSceneProxy;
class FRenderer
{
public:
	FRenderer(FSceneProxy* InSceneProxy);
	virtual ~FRenderer();

	virtual void BeginFrame() = 0;
	virtual void DrawFrame() = 0;
	virtual void EndFrame() = 0;

protected:
	FSceneProxy* mSceneProxy = nullptr;

};

}