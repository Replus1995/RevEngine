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

class FRHIVertexArray;
struct MeshPrimitive
{
public:
	EMeshDrawMode DrawMode;
	uint32 MaterialIndex;
	Ref<FRHIVertexArray> VertexData;

	MeshPrimitive();
	MeshPrimitive(uint32 InMaterialIndex, const Ref<FRHIVertexArray>& InVertexData, EMeshDrawMode InDrawMode = EMeshDrawMode::TRIANGLES);
	MeshPrimitive(const MeshPrimitive& InPrimitive);
	MeshPrimitive(MeshPrimitive&& InPrimitive) noexcept;

	MeshPrimitive& operator=(const MeshPrimitive& InPrimitive);
	MeshPrimitive& operator=(MeshPrimitive&& InPrimitive) noexcept;

	bool Valid() const;
};

}