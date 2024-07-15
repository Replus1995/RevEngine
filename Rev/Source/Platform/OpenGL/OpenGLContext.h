#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/Mesh/MeshPrimitive.h"
#include <glad/glad.h>

namespace Rev
{

class OpenGLContext : public FRHIContext
{
public:
	virtual void Init() override;
	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) override;
	virtual void SetClearColor(const Math::FLinearColor& color) override;
	virtual void Clear() override;

	virtual void PrepareMaterial(const Material* InMaterial) override;
	virtual void DrawPrimitive(const MeshPrimitive* InPrimitive) override;

private:
	GLenum TranslateDrawMode(EMeshDrawMode InDrawMode);
	GLenum TranslateIndexType(uint32 InStride);
};

}