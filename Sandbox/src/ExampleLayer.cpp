#include "ExampleLayer.h"
#include <Pine/World/Scene.h>
#include <Pine/World/Component/Component.h>
#include <Pine/Asset/StaticMeshAsset.h>

using namespace Pine;

ExampleLayer::ExampleLayer()
	: SceneLayer("ExampleLayer")
{
	//Create Sandbox Scene
	mScene = CreateRef<Pine::Scene>();

	auto meshEntity = mScene->CreateEntity();
	auto& meshComp = meshEntity.AddComponent<StaticMeshComponent>();
	meshComp.StaticMesh = StaticMeshAsset::CreateBox();
	auto& transformComp = meshEntity.AddComponent<TransformComponent>();
	transformComp.Location = glm::vec3(0,0,-5);
}

ExampleLayer::~ExampleLayer()
{
	
}
