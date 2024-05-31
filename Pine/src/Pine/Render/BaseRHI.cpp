#include "pinepch.h"
#include "Pine/Render/BaseRHI.h"
#include "Pine/Core/Log.h"
#include "Pine/Core/Application.h"

#include "Platform/OpenGL/OpenGLRHI.h"

namespace Pine
{
	std::unique_ptr<BaseRHI> BaseRHI::Create()
	{
		switch (Application::GetRenderAPI())
		{
		case Pine::ERenderAPI::None:
			PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!");
			return nullptr;
		case Pine::ERenderAPI::OpenGL:
			return std::make_unique<OpenGLRHI>();
		case Pine::ERenderAPI::Vulkan:
			PE_CORE_ASSERT(false, "ERenderAPI::Vulkan is currently not supported!");
			return nullptr;
		}

		PE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}