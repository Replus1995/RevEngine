#pragma once
#include "Rev/Render/Mesh.h"

namespace Rev
{

class MeshBuilder
{
public:
	virtual ~MeshBuilder() = default;
	virtual Ref<Mesh> Build() = 0;
protected:
	MeshBuilder() = default;
};

}