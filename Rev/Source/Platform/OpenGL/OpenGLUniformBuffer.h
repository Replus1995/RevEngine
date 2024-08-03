#pragma once
#include "Rev/Render/RHI/RHIBuffer.h"

namespace Rev
{

class FOpenGLUniformBuffer : public FRHIUniformBuffer
{
public:
	FOpenGLUniformBuffer(uint32 size);
	virtual ~FOpenGLUniformBuffer();
	virtual const void* GetNativeHandle() const override { return &mHandle; }
	virtual void UpdateSubData(const void* data, uint32 size, uint32 offset = 0) override;
private:
	uint32_t mHandle = 0;
};

}