#pragma once
#include <glad/glad.h>
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/Mesh/MeshPrimitive.h"

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
	virtual void DrawVertices(const Ref<FRHIVertexArray>& InVertexArray, EDrawMode InDrawMode) override;

private:
	GLenum TranslateDrawMode(EDrawMode InDrawMode);
	GLenum TranslateIndexType(uint32 InStride);
};

}