#include "pinepch.h"
#include "Pine/Render/UniformBuffer.h"

namespace Pine
{

std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLUniformBuffer>(size, binding);
	}

	PE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}


}