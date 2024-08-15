#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"

//Private
#include "OpenGL/OpenGLContext.h"
#include "Vulkan/VkContext.h"

namespace Rev
{
	Scope<FRHIContext> FRHIContext::Create()
	{
		switch (GetRenderAPI())
		{
		case Rev::ERenderAPI::None:
			REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!");
			return nullptr;
		case Rev::ERenderAPI::OpenGL:
			return CreateScope<FOpenGLContext>();
		case Rev::ERenderAPI::Vulkan:
			return CreateScope<FVkContext>();
		}

		REV_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}