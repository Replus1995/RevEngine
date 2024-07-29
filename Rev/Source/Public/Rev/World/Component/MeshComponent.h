#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class StaticMesh;
struct StaticMeshComponent
{
	Ref<class StaticMesh> StaticMesh;

	StaticMeshComponent() = default;
	StaticMeshComponent(const StaticMeshComponent& other){ this->StaticMesh = other.StaticMesh; }

};

}