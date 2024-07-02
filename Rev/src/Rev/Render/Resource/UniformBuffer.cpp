#include "pinepch.h"
#include "UniformBuffer.h"
#include "Pine/Render/RenderCore.h"
#include "Pine/Core/Assert.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Pine
{

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
	switch (GetRenderAPI())
	{
		case ERenderAPI::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
		case ERenderAPI::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
	}

	PE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}


}