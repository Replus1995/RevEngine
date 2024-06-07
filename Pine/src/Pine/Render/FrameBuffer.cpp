#include "pinepch.h"
#include "Pine/Render/Framebuffer.h"
#include "Pine/Render/RenderCore.h"
#include "Pine/Core/Assert.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Pine {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferDescription& spec)
	{
		switch (GetRenderAPI())
		{
		case ERenderAPI::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
		case ERenderAPI::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
		}

		PE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}