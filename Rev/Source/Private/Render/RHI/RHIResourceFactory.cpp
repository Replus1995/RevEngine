#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

//OpenGL impl headers
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Rev
{

namespace OpenGLImpl
{

}


Ref<FRHITexture> FRHIResourceFactory::CreateTexture(const FRHITextureDesc& InDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return FOpenGLTexture::Create(InDesc);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

}