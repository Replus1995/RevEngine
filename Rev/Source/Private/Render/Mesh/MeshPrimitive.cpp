#include "Rev/Render/Mesh/MeshPrimitive.h"

namespace Rev
{

FMeshPrimitive::FMeshPrimitive()
	: MaterialIndex(0)
	, VertexData(nullptr)
	, DrawMode(EMeshDrawMode::TRIANGLES)
{
}

FMeshPrimitive::FMeshPrimitive(uint32 InMaterialIndex, const Ref<FVertexArray>& InVertexData, EMeshDrawMode InDrawMode)
	: MaterialIndex(InMaterialIndex)
	, VertexData(InVertexData)
	, DrawMode(InDrawMode)
{
}

FMeshPrimitive::FMeshPrimitive(const FMeshPrimitive& InPrimitive)
	: MaterialIndex(InPrimitive.MaterialIndex)
	, VertexData(InPrimitive.VertexData)
	, DrawMode(InPrimitive.DrawMode)
{
}

FMeshPrimitive::FMeshPrimitive(FMeshPrimitive&& InPrimitive) noexcept
	: MaterialIndex(InPrimitive.MaterialIndex)
	, VertexData(std::move(InPrimitive.VertexData))
	, DrawMode(InPrimitive.DrawMode)
{
}

FMeshPrimitive& FMeshPrimitive::operator=(const FMeshPrimitive& InPrimitive)
{
	MaterialIndex = InPrimitive.MaterialIndex;
	VertexData = InPrimitive.VertexData;
	DrawMode = InPrimitive.DrawMode;
	return *this;
}

FMeshPrimitive& FMeshPrimitive::operator=(FMeshPrimitive&& InPrimitive) noexcept
{
	MaterialIndex = InPrimitive.MaterialIndex;
	VertexData = std::move(InPrimitive.VertexData);
	DrawMode = InPrimitive.DrawMode;
	return *this;
}

bool FMeshPrimitive::Valid() const
{
	return VertexData != nullptr;
}

}


