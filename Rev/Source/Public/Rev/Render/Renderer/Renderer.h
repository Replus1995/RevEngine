#pragma once
#include "Rev/Render/RenderCore.h"

namespace Rev
{

struct FRenderContext
{
	uint32 Width;
	uint32 Height;
	class SceneRenderProxy* SceneProxy;
};

class FRenderer
{
public:
	FRenderer(const Ref<FRenderContext>& InContext);
	virtual ~FRenderer() = default;

	virtual void BeginFrame() = 0;
	virtual void DrawFrame() = 0;
	virtual void EndFrame() = 0;

	const Ref<FRenderContext>& GetContext() const { return mContext; }

protected:
	Ref<FRenderContext> mContext;

};

}