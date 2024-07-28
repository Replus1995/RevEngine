#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Mesh/MeshPrimitive.h"
#include <vector>

namespace Rev
{

class Material;
class FRHIVertexArray;
class REV_API Mesh
{
public:
	virtual ~Mesh() = default;

	virtual const Ref<Material>& GetMaterial(uint32 MaterialIndex) const = 0;
	virtual uint32 GetMaterialCount() const = 0;
	virtual std::vector<const FMeshPrimitive*> GetPrimitive(uint32 MaterialIndex) const = 0;
	virtual uint32 GetPrimitiveCount() const = 0;

protected:
	Mesh();
};

}