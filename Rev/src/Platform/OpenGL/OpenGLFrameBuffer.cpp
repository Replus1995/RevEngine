#include "OpenGLFramebuffer.h"
#include "Rev/Core/Assert.h"

namespace Rev {

static const uint32_t sMaxFramebufferSize = 8192;

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDescription& desc)
	: mDesc(desc)
{
	for (auto texDesc : mDesc.Attachments.Attachments)
	{
		if (texDesc.TextureFormat != FramebufferTextureFormat::DEPTH24STENCIL8)
			mColorAttachmentDescs.emplace_back(texDesc);
		else
			mDepthAttachmentDesc = texDesc;
	}

	Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	glDeleteFramebuffers(1, &mHandle);
	glDeleteTextures(mColorAttachmentHandles.size(), mColorAttachmentHandles.data());
	glDeleteTextures(1, &mDepthAttachmentHandle);
}

void OpenGLFramebuffer::Invalidate()
{
	if (mHandle)
	{
		glDeleteFramebuffers(1, &mHandle);
		glDeleteTextures(mColorAttachmentHandles.size(), mColorAttachmentHandles.data());
		glDeleteTextures(1, &mDepthAttachmentHandle);

		mColorAttachmentHandles.clear();
		mDepthAttachmentHandle = 0;
	}

	glCreateFramebuffers(1, &mHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);

	bool multisample = mDesc.Samples > 1;

	// Attachments
	if (mColorAttachmentDescs.size())
	{
		mColorAttachmentHandles.resize(mColorAttachmentDescs.size());
		CreateTextures(multisample, mColorAttachmentHandles.data(), mColorAttachmentHandles.size());

		for (size_t i = 0; i < mColorAttachmentHandles.size(); i++)
		{
			BindTexture(multisample, mColorAttachmentHandles[i]);
			switch (mColorAttachmentDescs[i].TextureFormat)
			{
			case FramebufferTextureFormat::RGBA8:
				AttachColorTexture(mColorAttachmentHandles[i], mDesc.Samples, GL_RGBA8, GL_RGBA, mDesc.Width, mDesc.Height, i);
				break;
			case FramebufferTextureFormat::R32I:
				AttachColorTexture(mColorAttachmentHandles[i], mDesc.Samples, GL_R32I, GL_RED_INTEGER, mDesc.Width, mDesc.Height, i);
				break;
			}
		}
	}

	if (mDepthAttachmentDesc.TextureFormat != FramebufferTextureFormat::None)
	{
		CreateTextures(multisample, &mDepthAttachmentHandle, 1);
		BindTexture(multisample, mDepthAttachmentHandle);
		switch (mDepthAttachmentDesc.TextureFormat)
		{
		case FramebufferTextureFormat::DEPTH24STENCIL8:
			AttachDepthTexture(mDepthAttachmentHandle, mDesc.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, mDesc.Width, mDesc.Height);
			break;
		}
	}

	if (mColorAttachmentHandles.size() > 1)
	{
		RE_CORE_ASSERT(mColorAttachmentHandles.size() <= 4);
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(mColorAttachmentHandles.size(), buffers);
	}
	else if (mColorAttachmentHandles.empty())
	{
		// Only depth-pass
		glDrawBuffer(GL_NONE);
	}

	RE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	glViewport(0, 0, mDesc.Width, mDesc.Height);
}

void OpenGLFramebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0 || width > sMaxFramebufferSize || height > sMaxFramebufferSize)
	{
		RE_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
		return;
	}
	mDesc.Width = width;
	mDesc.Height = height;

	Invalidate();
}

int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	RE_CORE_ASSERT(attachmentIndex < mColorAttachmentHandles.size());

	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	int pixelData = 0;
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
	return pixelData;

}

void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
	RE_CORE_ASSERT(attachmentIndex < mColorAttachmentHandles.size());

	auto& desc = mColorAttachmentDescs[attachmentIndex];
	glClearTexImage(mColorAttachmentHandles[attachmentIndex], 0, GetGLFormat(desc.TextureFormat), GL_INT, &value);
}

uint32_t OpenGLFramebuffer::GetAttachmentHandle(uint32_t index) const
{
	RE_CORE_ASSERT(index < mColorAttachmentHandles.size());
	return mColorAttachmentHandles[index];
}

GLenum OpenGLFramebuffer::GetGLFormat(FramebufferTextureFormat format)
{
	switch (format)
	{
	case FramebufferTextureFormat::RGBA8:			return GL_RGBA8;
	case FramebufferTextureFormat::R32I:			return GL_RED_INTEGER;
	case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL;
	}

	RE_CORE_ASSERT(false);
	return 0;
}

void OpenGLFramebuffer::CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
{
	GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glCreateTextures(target, count, outID);
}

void OpenGLFramebuffer::BindTexture(bool multisampled, uint32_t id)
{
	GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glBindTexture(target, id);
}

void OpenGLFramebuffer::AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
{
	bool multisampled = samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target, id, 0);
}

void OpenGLFramebuffer::AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
{
	bool multisampled = samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
	}
	else
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, target, id, 0);
}

}