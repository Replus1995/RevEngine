#pragma once
#include "OpenGLTexture.h"
#include <glad/glad.h>

namespace Rev
{

class FOpenGLTexture2D : public FOpenGLTexture
{
public:
	FOpenGLTexture2D(const FTextureDesc&  InDesc);
	virtual ~FOpenGLTexture2D();

	virtual void UpdateData(const void* InData, uint32 InSize) override;
	virtual void ClearData() override;
	virtual void Bind(uint32 InUnit = 0) const override;

private:
	void CreateResource();
};


}