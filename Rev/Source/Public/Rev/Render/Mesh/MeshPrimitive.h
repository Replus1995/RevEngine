#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
enum class EMeshDrawMode : uint8
{
	POINTS = 0,
	LINE = 1,
	LINE_LOOP = 2,
	LINE_STRIP = 3,
	TRIANGLES = 4,
	TRIANGLE_STRIP = 5,
	TRIANGLE_FAN = 6
};

class FVertexArray;
struct FMeshPrimitive
{
public:
	uint32 MaterialIndex;
	Ref<FVertexArray> VertexData;
	EMeshDrawMode DrawMode;

	FMeshPrimitive();
	FMeshPrimitive(uint32 InMaterialIndex, const Ref<FVertexArray>& InVertexData, EMeshDrawMode InDrawMode = EMeshDrawMode::TRIANGLES);
	FMeshPrimitive(const FMeshPrimitive& InPrimitive);
	FMeshPrimitive(FMeshPrimitive&& InPrimitive) noexcept;

	FMeshPrimitive& operator=(const FMeshPrimitive& InPrimitive);
	FMeshPrimitive& operator=(FMeshPrimitive&& InPrimitive) noexcept;

	bool Valid() const;
};

}