#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIPrimitive.h"

namespace Rev
{

struct FMeshPrimitive
{
public:
	uint32 MaterialIndex;
	Ref<FRHIPrimitive> PrimitiveData;

	FMeshPrimitive();
	FMeshPrimitive(uint32 InMaterialIndex, const Ref<FRHIPrimitive>& InVertexData);
	FMeshPrimitive(const FMeshPrimitive& InPrimitive);
	FMeshPrimitive(FMeshPrimitive&& InPrimitive) noexcept;

	FMeshPrimitive& operator=(const FMeshPrimitive& InPrimitive);
	FMeshPrimitive& operator=(FMeshPrimitive&& InPrimitive) noexcept;

	bool Valid() const;
};

}