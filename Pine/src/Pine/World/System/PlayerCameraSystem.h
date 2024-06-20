#pragma once
#include "Pine/World/System/System.h"
#include "Pine/Render/Camera.h"

namespace Pine
{

class PlayerCameraSystem : public ISystem
{
	DECLARE_SYSTEM(PlayerCameraSystem)
public:
	void GetCameraMatrix(glm::mat4& projMat, glm::mat4& viewMat) const;
};

}