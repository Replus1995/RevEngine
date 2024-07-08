#include "OpenGLTexture.h"
#include "Rev/Core/Assert.h"

#include "OpenGLTexture2D.h"

namespace Rev
{

uint32 FOpenGLTexture::GetGLPixelSize(GLenum internalFormat)
{
	switch (internalFormat)
	{
	case GL_R8: return 1;
	case GL_R16: return 2;
	case GL_RGB8: return 3;
	case GL_RGBA8: return 4;
	case GL_RGB32F: return 4 * 4;
	}
	RE_CORE_ASSERT(false, "Unknown GL Internal Format.");
	return 0;
}

Ref<FOpenGLTexture> FOpenGLTexture::Create(const FRHITextureDesc& InDesc)
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


