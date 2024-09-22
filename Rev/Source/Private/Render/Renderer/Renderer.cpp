#include "Rev/Render/Renderer/Renderer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FRenderer::FRenderer(const Ref<FRenderContext>& InContext)
	: mContext(InContext)
{
	RE_CORE_ASSERT(mContext != nullptr);
}

}