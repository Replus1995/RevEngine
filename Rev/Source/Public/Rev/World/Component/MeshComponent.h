#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

struct StaticMeshComponent
{
	Ref<class FStaticMesh> StaticMesh;

	StaticMeshComponent() = default;
	StaticMeshComponent(const StaticMeshComponent& other){ this->StaticMesh = other.StaticMesh; }

};

}