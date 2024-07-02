#pragma once
#include "Rev/Core/Base.h"
#include <vector>

namespace Rev
{
class VertexArray;
class Material;
class REV_API Mesh
{
public:
	virtual ~Mesh() = default;

	virtual const Ref<Material>& GetMaterial(uint32 index) const = 0;
	virtual uint32 GetMaterialCount() const = 0;
	virtual std::vector<const VertexArray*> GetVertexArrayByIndex(uint32 index) const = 0;

protected:
	Mesh() = default;
};

class StaticMesh;
class REV_API MeshUtils
{
public:
	static void SetDefaultMaterial(const Ref<Material>& mat);

	static Ref<StaticMesh> CreateBox();
private:
	static Ref<Material>& GetDefaultMaterial();
};


}