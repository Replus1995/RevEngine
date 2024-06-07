#include "pinepch.h"
#include "Pine/Render/BaseRHI.h"
#include "Pine/Core/Assert.h"
#include "Pine/Core/Application.h"

#include "Platform/OpenGL/OpenGLRHI.h"

namespace Pine
{
	Scope<BaseRHI> BaseRHI::Create()
	{
		switch (GetRenderAPI())
		{
		case Pine::ERenderAPI::None:
			PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!");
			return nullptr;
		case Pine::ERenderAPI::OpenGL:
			return CreateScope<OpenGLRHI>();
		case Pine::ERenderAPI::Vulkan:
			PE_CORE_ASSERT(false, "ERenderAPI::Vulkan is currently not supported!");
			return nullptr;
		}

		PE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}