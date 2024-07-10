#pragma once
#include "Rev/Render/RHI/RHIBuffer.h"

namespace Rev
{

class FOpenGLUniformBuffer : public FRHIUniformBuffer
{
public:
	FOpenGLUniformBuffer(uint32_t size, uint32_t binding);
	virtual ~FOpenGLUniformBuffer();
	virtual void* GetNativeHandle() override { return &mHandle; }
	virtual void UpdateData(const void* data, uint32_t size, uint32_t offset = 0) override;
private:
	uint32_t mHandle = 0;
};

}