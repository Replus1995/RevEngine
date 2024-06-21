#pragma once
#include "Pine/Core/Base.h"
#include <vector>

namespace Pine
{
class VertexArray;
class Material;
class PINE_API Mesh
{
public:
	virtual ~Mesh() = default;

	virtual const Ref<Material>& GetMaterial(uint32 index) const = 0;
	virtual uint32 GetMaterialCount() const = 0;
	virtual std::vector<const VertexArray*> GetVertexArrayByIndex(uint32 index) const = 0;

protected:
	Mesh() = default;
};



}