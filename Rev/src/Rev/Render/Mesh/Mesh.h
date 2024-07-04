#pragma once
#include "Rev/Core/Base.h"
#include <vector>
#include <string>

namespace Rev
{

//Follows tinygltf
enum class MeshDrawMode
{
	POINTS = 0,
	LINE = 1,
	LINE_LOOP = 2,
	LINE_STRIP = 3,
	TRIANGLES = 4,
	TRIANGLE_STRIP = 5,
	TRIANGLE_FAN = 6
};

class Material;
class VertexArray;
class REV_API Mesh
{
public:
	virtual ~Mesh() = default;

	virtual const Ref<Material>& GetMaterial(uint32 index) const = 0;
	virtual uint32 GetMaterialCount() const = 0;
	virtual std::vector<const VertexArray*> GetVertexArrayByIndex(uint32 index) const = 0;

	void SetName(const std::string& InName);
	const std::string& GetName() const;
	void SetDrawMode(MeshDrawMode InMode);

protected:
	Mesh();
	std::string mName;
	MeshDrawMode mMode;
};

}