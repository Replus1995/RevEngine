#pragma once
#include "Rev/Render/Resource/FrameBuffer.h"
#include <glad/glad.h>

namespace Rev
{

class OpenGLFramebuffer : public Framebuffer
{
public:
	OpenGLFramebuffer(const FramebufferDescription& desc);
	virtual ~OpenGLFramebuffer();

	void Invalidate();

	virtual void Bind() override;
	virtual void Unbind() override;

	virtual void Resize(uint32_t width, uint32_t height) override;
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

	virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

	virtual uint32_t GetAttachmentHandle(uint32_t index = 0) const override;

	virtual const FramebufferDescription& GetDesc() const override { return mDesc; }

public:
	static GLenum GetGLFormat(FramebufferTextureFormat format);

private:
	static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count);
	static void BindTexture(bool multisampled, uint32_t id);

	static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index);
	static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height);

private:
	uint32_t mHandle = 0;
	FramebufferDescription mDesc;

	std::vector<FramebufferTextureDescription> mColorAttachmentDescs;
	FramebufferTextureDescription mDepthAttachmentDesc = FramebufferTextureFormat::None;

	std::vector<uint32_t> mColorAttachmentHandles;
	uint32_t mDepthAttachmentHandle = 0;

};

}