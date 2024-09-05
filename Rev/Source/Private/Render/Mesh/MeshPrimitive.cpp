#include "Rev/Render/Mesh/MeshPrimitive.h"

namespace Rev
{

FMeshPrimitive::FMeshPrimitive()
	: MaterialIndex(0)
	, VertexData(nullptr)
{
}

FMeshPrimitive::FMeshPrimitive(uint32 InMaterialIndex, const Ref<FRHIVertexArray>& InVertexData)
	: MaterialIndex(InMaterialIndex)
	, VertexData(InVertexData)
{
}

FMeshPrimitive::FMeshPrimitive(const FMeshPrimitive& InPrimitive)
	: MaterialIndex(InPrimitive.MaterialIndex)
	, VertexData(InPrimitive.VertexData)
{
}

FMeshPrimitive::FMeshPrimitive(FMeshPrimitive&& InPrimitive) noexcept
	: MaterialIndex(InPrimitive.MaterialIndex)
	, VertexData(std::move(InPrimitive.VertexData))
{
}

FMeshPrimitive& FMeshPrimitive::operator=(const FMeshPrimitive& InPrimitive)
{
	MaterialIndex = InPrimitive.MaterialIndex;
	VertexData = InPrimitive.VertexData;
	return *this;
}

FMeshPrimitive& FMeshPrimitive::operator=(FMeshPrimitive&& InPrimitive) noexcept
{
	MaterialIndex = InPrimitive.MaterialIndex;
	VertexData = std::move(InPrimitive.VertexData);
	return *this;
}

bool FMeshPrimitive::Valid() const
{
	return VertexData != nullptr;
}

}


