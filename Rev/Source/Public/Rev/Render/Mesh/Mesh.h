#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Mesh/MeshPrimitive.h"
#include <vector>
#include <string>

namespace Rev
{

class Material;
class FVertexArray;
class REV_API Mesh
{
public:
	virtual ~Mesh() = default;

	virtual const Ref<Material>& GetMaterial(uint32 MaterialIndex) const = 0;
	virtual uint32 GetMaterialCount() const = 0;
	virtual std::vector<const FMeshPrimitive*> GetPrimitive(uint32 MaterialIndex) const = 0;
	virtual uint32 GetPrimitiveCount() const = 0;

	void SetName(const std::string& InName);
	const std::string& GetName() const;

protected:
	Mesh();
	std::string mName;
};

}