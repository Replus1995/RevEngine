#include "Rev/Render/Mesh/MeshPrimitive.h"
#include "Rev/Render/RHI/RHIPrimitive.h"

namespace Rev
{

FMeshPrimitive::FMeshPrimitive()
	: MaterialIndex(0)
	, PrimitiveData(nullptr)
{
}

FMeshPrimitive::FMeshPrimitive(uint32 InMaterialIndex, const Ref<FRHIPrimitive>& InPrimitiveData)
	: MaterialIndex(InMaterialIndex)
	, PrimitiveData(InPrimitiveData)
{
}

FMeshPrimitive::FMeshPrimitive(const FMeshPrimitive& InPrimitive)
	: MaterialIndex(InPrimitive.MaterialIndex)
	, PrimitiveData(InPrimitive.PrimitiveData)
{
}

FMeshPrimitive::FMeshPrimitive(FMeshPrimitive&& InPrimitive) noexcept
	: MaterialIndex(InPrimitive.MaterialIndex)
	, PrimitiveData(std::move(InPrimitive.PrimitiveData))
{
}

FMeshPrimitive& FMeshPrimitive::operator=(const FMeshPrimitive& InPrimitive)
{
	MaterialIndex = InPrimitive.MaterialIndex;
	PrimitiveData = InPrimitive.PrimitiveData;
	return *this;
}

FMeshPrimitive& FMeshPrimitive::operator=(FMeshPrimitive&& InPrimitive) noexcept
{
	MaterialIndex = InPrimitive.MaterialIndex;
	PrimitiveData = std::move(InPrimitive.PrimitiveData);
	return *this;
}

bool FMeshPrimitive::Valid() const
{
	return PrimitiveData != nullptr;
}

}


