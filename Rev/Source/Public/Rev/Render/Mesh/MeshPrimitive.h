#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
class FRHIVertexArray;
struct FMeshPrimitive
{
public:
	EDrawMode DrawMode;
	uint32 MaterialIndex;
	Ref<FRHIVertexArray> VertexData;

	FMeshPrimitive();
	FMeshPrimitive(uint32 InMaterialIndex, const Ref<FRHIVertexArray>& InVertexData, EDrawMode InDrawMode = EDrawMode::DM_Triangles);
	FMeshPrimitive(const FMeshPrimitive& InPrimitive);
	FMeshPrimitive(FMeshPrimitive&& InPrimitive) noexcept;

	FMeshPrimitive& operator=(const FMeshPrimitive& InPrimitive);
	FMeshPrimitive& operator=(FMeshPrimitive&& InPrimitive) noexcept;

	bool Valid() const;
};

}