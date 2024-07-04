#include "Rev/Render/Resource/Framebuffer.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Rev {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferDescription& spec)
	{
		switch (GetRenderAPI())
		{
		case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
		case ERenderAPI::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
		}

		RE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}