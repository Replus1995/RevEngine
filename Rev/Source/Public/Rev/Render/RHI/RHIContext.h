#pragma once
#include <memory>
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"

namespace Rev
{
class Material;
class MeshPrimitive;
class FRHIVertexArray;
class FRHIContext
{
public:
	virtual ~FRHIContext() = default;

	virtual void Init() = 0;
	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;
	virtual void SetClearColor(const Math::FLinearColor& color) = 0;
	virtual void Clear() = 0;

	virtual void PrepareMaterial(const Material* InMaterial) = 0;
	virtual void DrawPrimitive(const MeshPrimitive* InPrimitive) = 0;

	static std::unique_ptr<FRHIContext> Create();
};
}