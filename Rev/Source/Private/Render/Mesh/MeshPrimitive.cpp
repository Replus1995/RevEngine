#include "Rev/Render/Mesh/MeshPrimitive.h"

namespace Rev
{

MeshPrimitive::MeshPrimitive()
	: MaterialIndex(0)
	, VertexData(nullptr)
	, DrawMode(EMeshDrawMode::TRIANGLES)
{
}

MeshPrimitive::MeshPrimitive(uint32 InMaterialIndex, const Ref<FRHIVertexArray>& InVertexData, EMeshDrawMode InDrawMode)
	: MaterialIndex(InMaterialIndex)
	, VertexData(InVertexData)
	, DrawMode(InDrawMode)
{
}

MeshPrimitive::MeshPrimitive(const MeshPrimitive& InPrimitive)
	: MaterialIndex(InPrimitive.MaterialIndex)
	, VertexData(InPrimitive.VertexData)
	, DrawMode(InPrimitive.DrawMode)
{
}

MeshPrimitive::MeshPrimitive(MeshPrimitive&& InPrimitive) noexcept
	: MaterialIndex(InPrimitive.MaterialIndex)
	, VertexData(std::move(InPrimitive.VertexData))
	, DrawMode(InPrimitive.DrawMode)
{
}

MeshPrimitive& MeshPrimitive::operator=(const MeshPrimitive& InPrimitive)
{
	MaterialIndex = InPrimitive.MaterialIndex;
	VertexData = InPrimitive.VertexData;
	DrawMode = InPrimitive.DrawMode;
	return *this;
}

MeshPrimitive& MeshPrimitive::operator=(MeshPrimitive&& InPrimitive) noexcept
{
	MaterialIndex = InPrimitive.MaterialIndex;
	VertexData = std::move(InPrimitive.VertexData);
	DrawMode = InPrimitive.DrawMode;
	return *this;
}

bool MeshPrimitive::Valid() const
{
	return VertexData != nullptr;
}

}


