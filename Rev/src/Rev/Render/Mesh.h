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

class StaticMesh;
class PINE_API MeshUtils
{
public:
	static void SetDefaultMaterial(const Ref<Material>& mat);

	static Ref<StaticMesh> CreateBox();
private:
	static Ref<Material>& GetDefaultMaterial();
};


}