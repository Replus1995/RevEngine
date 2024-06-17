#pragma once
#include "Pine/Core/Base.h"

namespace Pine
{
class Scene;
class SceneRenderProxy
{
public:
	SceneRenderProxy() = default;
	~SceneRenderProxy() = default;

	void Prepare(const Ref<Scene>& scene);
	void Draw();

};

}
