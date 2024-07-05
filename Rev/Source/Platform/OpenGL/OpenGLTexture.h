#pragma once
#include "Rev/Render/RHI/RHITexture.h"
#include <glad/glad.h>

namespace Rev
{

class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(const TextureDescription& desc);
	OpenGLTexture2D(const std::string& path);
	virtual ~OpenGLTexture2D();

	virtual const TextureDescription& GetDesc() const override { return mDesc; }

	virtual uint32_t GetWidth() const override { return mWidth; }
	virtual uint32_t GetHeight() const override { return mHeight; }
	virtual uint32_t GetHandle() const override { return mHandle; }

	virtual const std::string& GetPath() const override { return mFilePath; }

	virtual void SetData(void* data, uint32_t size) override;

	virtual void Bind(uint32_t slot = 0) const override;

	virtual bool IsLoaded() const override { return mLoaded; }

	virtual bool operator==(const Texture& other) const override
	{
		return mHandle == other.GetHandle();
	}
public:
	static uint32_t GetGLPixelSize(GLenum internalFormat);

private:
	void InitFormatInfo(ImageFormat format);
	void CreateTexture();

private:
	TextureDescription mDesc;

	std::string mFilePath;
	bool mLoaded = false;
	uint32_t mWidth = 0, mHeight = 0;
	uint32_t mHandle = 0;
	GLenum mInternalFormat = 0, mDataFormat = 0, mDataType = 0;
};


}