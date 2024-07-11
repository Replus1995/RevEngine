#include "OpenGLTexture.h"
#include "Rev/Core/Assert.h"

#include "OpenGLTexture2D.h"

namespace Rev
{

Ref<FOpenGLTexture> FOpenGLTexture::Create(const FTextureDesc& InDesc)
{
	switch (InDesc.Dimension)
	{
	case ETextureDimension::Texture2D:
		return CreateRef<FOpenGLTexture2D>(InDesc);
	case ETextureDimension::Texture2DArray:
	case ETextureDimension::Texture3D:
	case ETextureDimension::TextureCube:
	case ETextureDimension::TextureCubeArray:
	default:
		break;
	}
	RE_CORE_ASSERT(false, "Unsupported Texture Dimension!");
	return nullptr;
}

}


