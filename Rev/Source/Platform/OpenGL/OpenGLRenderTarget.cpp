#include "OpenGLRenderTarget.h"

namespace Rev
{

const uint16 FOpenGLRenderTarget::sMaxRenderTargetSize = 8192;

FOpenGLRenderTarget::FOpenGLRenderTarget(const FRenderTargetDesc& InDesc)
	: FRHIRenderTarget(InDesc)
{
	if (!IsEmptyTarget())
	{
		REV_CORE_ASSERT(InDesc.Width > 0 && InDesc.Width <= sMaxRenderTargetSize, "Invalid render target width");
		REV_CORE_ASSERT(InDesc.Height > 0 && InDesc.Height <= sMaxRenderTargetSize, "Invalid render target height");
	}
	CreateResource();
}

FOpenGLRenderTarget::~FOpenGLRenderTarget()
{
	ReleaseResource();
}

void FOpenGLRenderTarget::ResizeTargets(uint16 InWidth, uint16 InHeight)
{
	if(IsEmptyTarget())
		return;

	if (InWidth == 0 || InHeight == 0 || InWidth > sMaxRenderTargetSize || InHeight > sMaxRenderTargetSize)
	{
		REV_CORE_WARN("Attempted to rezize framebuffer to {0}, {1} failed", InWidth, InHeight);
		return;
	}
	if (mDesc.Width == InWidth && mDesc.Height == InHeight)
		return;

	mDesc.Width = InWidth;
	mDesc.Height = InHeight;
	ReleaseResource();
	CreateResource();
}

void FOpenGLRenderTarget::ClearTarget(ERenderTargetAttachment Index)
{
	/*if (Index == RTA_DepthStencilAttachment)
	{
		if (mDepthStencilAttachment.Texture)
		{
			if(mDepthStencilAttachment.ArrayIndex == -1)
				mDepthStencilAttachment.Texture->ClearAllData();
			else
				mDepthStencilAttachment.Texture->ClearLayerData(mDepthStencilAttachment.MipLevel, mDepthStencilAttachment.ArrayIndex);
		}
	}
	else if (Index >= 0 && Index < RTA_MaxColorAttachments)
	{
		if (mColorAttachments[Index].Texture)
		{
			if (mColorAttachments[Index].ArrayIndex == -1)
				mColorAttachments[Index].Texture->ClearAllData();
			else
				mColorAttachments[Index].Texture->ClearLayerData(mColorAttachments[Index].MipLevel, mColorAttachments[Index].ArrayIndex);
		}
			
	}*/
	if (Index < RTA_MaxColorAttachments)
	{
		if (mColorAttachments[Index].Texture)
		{
			auto& LinearClearColor = mColorAttachments[Index].Texture->GetDesc().ClearColor.RGBA;
			GLenum DataType = mColorAttachments[Index].Texture->GetOpenGLFormatData().DataType;
			switch (DataType)
			{
			case GL_BYTE:
			{
				const float ColorSacle = 128.0f;
				GLint IntClearColor[4] = { 
					GLint(LinearClearColor.R * ColorSacle), 
					GLint(LinearClearColor.G * ColorSacle), 
					GLint(LinearClearColor.B * ColorSacle), 
					GLint(LinearClearColor.A * ColorSacle)
				};
				glClearNamedFramebufferiv(mHandle, GL_COLOR, Index, IntClearColor);
				break;
			}
			case GL_UNSIGNED_BYTE:
			{
				const float ColorSacle = 255.0f;
				GLuint IntClearColor[4] = { 
					GLuint(LinearClearColor.R * ColorSacle), 
					GLuint(LinearClearColor.G * ColorSacle), 
					GLuint(LinearClearColor.B * ColorSacle), 
					GLuint(LinearClearColor.A * ColorSacle)
				};
				glClearNamedFramebufferuiv(mHandle, GL_COLOR, Index, IntClearColor);
				break;
			}
			case GL_SHORT:
			{
				const float ColorSacle = 32767.0f;
				GLint IntClearColor[4] = {
					GLint(LinearClearColor.R * ColorSacle),
					GLint(LinearClearColor.G * ColorSacle),
					GLint(LinearClearColor.B * ColorSacle),
					GLint(LinearClearColor.A * ColorSacle)
				};
				glClearNamedFramebufferiv(mHandle, GL_COLOR, Index, IntClearColor);
				break;
			}
			case GL_UNSIGNED_SHORT:
			{
				const float ColorSacle = 65535.0f;
				GLuint IntClearColor[4] = {
					GLuint(LinearClearColor.R * ColorSacle),
					GLuint(LinearClearColor.G * ColorSacle),
					GLuint(LinearClearColor.B * ColorSacle),
					GLuint(LinearClearColor.A * ColorSacle)
				};
				glClearNamedFramebufferuiv(mHandle, GL_COLOR, Index, IntClearColor);
				break;
			}
			case GL_INT:
			{
				const double ColorSacle = 2147483647.0f;
				GLint IntClearColor[4] = {
					GLint(LinearClearColor.R * ColorSacle),
					GLint(LinearClearColor.G * ColorSacle),
					GLint(LinearClearColor.B * ColorSacle),
					GLint(LinearClearColor.A * ColorSacle)
				};
				glClearNamedFramebufferiv(mHandle, GL_COLOR, Index, IntClearColor);
				break;
			}
			case GL_UNSIGNED_INT:
			{
				const double ColorSacle = 4294967295.0f;
				GLuint IntClearColor[4] = {
					GLuint(LinearClearColor.R * ColorSacle),
					GLuint(LinearClearColor.G * ColorSacle),
					GLuint(LinearClearColor.B * ColorSacle),
					GLuint(LinearClearColor.A * ColorSacle)
				};
				glClearNamedFramebufferuiv(mHandle, GL_COLOR, Index, IntClearColor);
				break;
			}
			case GL_FLOAT:
				glClearNamedFramebufferfv(mHandle, GL_COLOR, Index, &LinearClearColor.R);
				break;
			default:
				REV_CORE_ASSERT(false, "Unknown render target texture data type");
				break;
			}
		}
	}
	else if (Index > RTA_MaxColorAttachments)
	{
		if (mDepthStencilAttachment.Texture)
		{
			float ClearDepth = mDepthStencilAttachment.Texture->GetDesc().ClearColor.Depth;
			uint32 ClearStencil = mDepthStencilAttachment.Texture->GetDesc().ClearColor.Stencil;

			if (Index == RTA_DepthAttachment)
			{
				glClearNamedFramebufferfv(mHandle, GL_DEPTH, 0, &ClearDepth);
			} 
			else //RTA_DepthStencilAttachment
			{
				glClearNamedFramebufferfi(mHandle, GL_DEPTH_STENCIL, 0, ClearDepth, ClearStencil);
			}
		}
	}
}

void FOpenGLRenderTarget::ClearTargets()
{
	for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
	{
		ClearTarget((ERenderTargetAttachment)i);
	}
	ClearTarget(RTA_DepthStencilAttachment);
}

const Ref<FRHITexture> FOpenGLRenderTarget::GetTargetTexture(ERenderTargetAttachment Index) const
{
	if (Index == RTA_DepthStencilAttachment)
	{
		return mDepthStencilAttachment.Texture;
	}
	else if (Index >= 0 && Index < RTA_MaxColorAttachments)
	{
		return mColorAttachments[Index].Texture;
	}
	return nullptr;
}

//void FOpenGLRenderTarget::Attach(ERenderTargetAttachment Index, const Ref<FRHITexture>& InTexture, uint8 InMipLevel, int32 InArrayIndex)
//{
//	if(!InTexture)
//		return;
//	if (InTexture->GetDesc().bSRGB)
//	{
//		REV_CORE_ERROR("SRGB texture is not allowed to be attached as render target");
//		return;
//	}
//
//	FAttachment& TargetAttachment = Index < RTA_MaxColorAttachments ? mColorAttachments[Index] : mDepthStencilAttachment;
//	GLenum AttachPoint = 0;
//	if (Index < RTA_MaxColorAttachments)
//		AttachPoint = GL_COLOR_ATTACHMENT0 + Index;
//	else if (Index == RTA_DepthAttachment)
//		AttachPoint = GL_DEPTH_ATTACHMENT;
//	else if (Index == RTA_DepthStencilAttachment)
//		AttachPoint = GL_DEPTH_STENCIL_ATTACHMENT;
//
//	REV_CORE_ASSERT(AttachPoint != 0);
//
//	GLuint TexHandle = *(const GLuint*)InTexture->GetNativeHandle();
//	if (InArrayIndex < 0)
//	{
//		glNamedFramebufferTexture(mHandle, AttachPoint, TexHandle, InMipLevel);
//		TargetAttachment.ArrayIndex = -1;
//	}
//	else
//	{
//		REV_CORE_ASSERT(InArrayIndex < InTexture->GetArraySize(), "ArrayIndex out of range");
//		glNamedFramebufferTextureLayer(mHandle, AttachPoint, TexHandle, InMipLevel, InArrayIndex);
//		TargetAttachment.ArrayIndex = InArrayIndex;
//	}
//	TargetAttachment.MipLevel = InMipLevel;
//	TargetAttachment.Texture = std::static_pointer_cast<FOpenGLTexture>(InTexture);
//	mAttachmentsDirty = true;
//}
//
//void FOpenGLRenderTarget::Detach(ERenderTargetAttachment Index)
//{
//	FAttachment& TargetAttachment = Index < RTA_MaxColorAttachments ? mColorAttachments[Index] : mDepthStencilAttachment;
//	GLenum AttachPoint = 0;
//	if (Index < RTA_MaxColorAttachments)
//		AttachPoint = GL_COLOR_ATTACHMENT0 + Index;
//	else if (Index == RTA_DepthAttachment)
//		AttachPoint = GL_DEPTH_ATTACHMENT;
//	else if (Index == RTA_DepthStencilAttachment)
//		AttachPoint = GL_DEPTH_STENCIL_ATTACHMENT;
//
//	if (AttachPoint != 0)
//	{
//		glNamedFramebufferTexture(mHandle, AttachPoint, 0, 0);
//	}
//}
//
//void FOpenGLRenderTarget::DetachAll()
//{
//	for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
//	{
//		Detach((ERenderTargetAttachment)i);
//	}
//	Detach(RTA_DepthStencilAttachment);
//}
//
//void FOpenGLRenderTarget::FlushAttach()
//{
//	if (mAttachmentsDirty)
//	{
//		std::vector<GLenum> ColorAttachPoints;
//		for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
//		{
//			if (mColorAttachments[i].Texture)
//			{
//				ColorAttachPoints.push_back(GL_COLOR_ATTACHMENT0 + i);
//			}
//		}
//		if (!ColorAttachPoints.empty())
//		{
//			glNamedFramebufferDrawBuffers(mHandle, ColorAttachPoints.size(), ColorAttachPoints.data());
//		}
//		else
//		{
//			glNamedFramebufferDrawBuffer(mHandle, GL_NONE);
//		}
//
//		mAttachmentsDirty = false;
//	}
//}

bool FOpenGLRenderTarget::IsEmptyTarget() const
{
	return mDesc.Dimension == ERenderTargetDimension::RenderTargetEmpty;
}

void FOpenGLRenderTarget::CreateResource()
{
	glCreateFramebuffers(1, &mHandle);
	if (!IsEmptyTarget())
	{
		for (uint32 i = 0; i < mDesc.NumColorTargets; i++)
		{
			if (auto ColorTexture = CreateColorTexture(mDesc.ColorTargets[i]); ColorTexture)
			{
				GLuint TexHandle = *(GLuint*)ColorTexture->GetNativeHandle();
				glNamedFramebufferTexture(mHandle, GL_COLOR_ATTACHMENT0 + i, TexHandle, 0);
				mColorAttachments[i].Texture = std::move(ColorTexture);
			}
		}
		if (auto DepthTexture = CreateDepthStencilTexture(mDesc.DepthStencilTarget); DepthTexture)
		{
			GLuint TexHandle = *(GLuint*)DepthTexture->GetNativeHandle();
			switch (DepthTexture->GetFormat())
			{
			case PF_ShadowDepth:
				glNamedFramebufferTexture(mHandle, GL_DEPTH_ATTACHMENT, TexHandle, 0);
				break;
			case PF_DepthStencil:
				glNamedFramebufferTexture(mHandle, GL_DEPTH_STENCIL_ATTACHMENT, TexHandle, 0);
				break;
			}
			mDepthStencilAttachment.Texture = std::move(DepthTexture);
		}
	}

	if (mDesc.NumColorTargets > 0)
	{
		std::vector<GLenum> ColorBuffers(mDesc.NumColorTargets);
		for (size_t i = 0; i < ColorBuffers.size(); i++)
		{
			ColorBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glNamedFramebufferDrawBuffers(mHandle, ColorBuffers.size(), ColorBuffers.data());
	}
	else
	{
		//Depth Only
		glNamedFramebufferDrawBuffer(mHandle, GL_NONE);
	}
	REV_CORE_ASSERT(glCheckNamedFramebufferStatus(mHandle, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "OpenGL RenderTarget Incomplete!");
}

void FOpenGLRenderTarget::ReleaseResource()
{
	if (mHandle)
	{
		glDeleteFramebuffers(1, &mHandle);
		if (!IsEmptyTarget())
		{
			for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
			{
				//mColorAttachments[i].MipLevel = 0;
				//mColorAttachments[i].ArrayIndex = -1;
				mColorAttachments[i].Texture.reset();
			}
			//mDepthStencilAttachment.MipLevel = 0;
			//mDepthStencilAttachment.ArrayIndex = -1;
			mDepthStencilAttachment.Texture.reset();
		}
	}
}

Ref<FOpenGLTexture> FOpenGLRenderTarget::CreateColorTexture(const FColorTargetDesc& InDesc)
{
	REV_CORE_ASSERT(!IsEmptyTarget());
	if(InDesc.Format == PF_Unknown)
		return nullptr;
	FTextureDesc TexDesc ((ETextureDimension)mDesc.Dimension, InDesc.Format, false, mDesc.Width, mDesc.Height, 0, mDesc.ArraySize, InDesc.ClearColor, 1, mDesc.NumSamples);
	return CreateOpenGLTexture(TexDesc, FSamplerDesc());
}

Ref<FOpenGLTexture> FOpenGLRenderTarget::CreateDepthStencilTexture(const FDepthStencilTargetDesc& InDesc)
{
	REV_CORE_ASSERT(!IsEmptyTarget());
	if (InDesc.Format != PF_ShadowDepth && InDesc.Format != PF_DepthStencil)
		return nullptr;
	FTextureDesc TexDesc((ETextureDimension)mDesc.Dimension, InDesc.Format, false, mDesc.Width, mDesc.Height, 0, mDesc.ArraySize, { InDesc.ClearDepth, InDesc.ClearStencil }, 1, mDesc.NumSamples);
	return CreateOpenGLTexture(TexDesc, FSamplerDesc());
}


}


