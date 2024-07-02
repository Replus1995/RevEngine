#include "UniformBuffer.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Rev
{

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
	switch (GetRenderAPI())
	{
		case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
		case ERenderAPI::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}


}