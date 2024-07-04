#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class Material;
class Mesh;
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