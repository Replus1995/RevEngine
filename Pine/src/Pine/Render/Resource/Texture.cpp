#include "pinepch.h"
#include "Texture.h"
#include "Pine/Render/RenderCore.h"
#include "Pine/Core/Assert.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Pine {

	Ref<Texture2D> Texture2D::Create(const TextureDescription& desc)
	{
		switch (GetRenderAPI())
		{
		case ERenderAPI::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
		case ERenderAPI::OpenGL:  return CreateRef<OpenGLTexture2D>(desc);
		}

		PE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (GetRenderAPI())
		{
		case ERenderAPI::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
		case ERenderAPI::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
		}

		PE_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}