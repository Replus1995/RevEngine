#pragma once
#include "Rev/Render/Component/Skybox.h"

namespace Rev
{

struct SkyComponent
{
	Skybox Skybox;

	SkyComponent() = default;
	SkyComponent(const SkyComponent& Other) = default;
};


}