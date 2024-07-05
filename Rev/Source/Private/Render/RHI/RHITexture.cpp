#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Rev {

	Ref<Texture2D> Texture2D::Create(const TextureDescription& desc)
	{
		switch (GetRenderAPI())
		{
		case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
		case ERenderAPI::OpenGL:  return CreateRef<OpenGLTexture2D>(desc);
		}

		RE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}