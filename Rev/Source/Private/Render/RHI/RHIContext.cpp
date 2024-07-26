#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"

//Private
#include "OpenGL/OpenGLContext.h"

namespace Rev
{
	Scope<FRHIContext> FRHIContext::Create()
	{
		switch (GetRenderAPI())
		{
		case Rev::ERenderAPI::None:
			RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!");
			return nullptr;
		case Rev::ERenderAPI::OpenGL:
			return CreateScope<OpenGLContext>();
		case Rev::ERenderAPI::Vulkan:
			RE_CORE_ASSERT(false, "ERenderAPI::Vulkan is currently not supported!");
			return nullptr;
		}

		RE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}