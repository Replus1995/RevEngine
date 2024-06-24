#include "ExampleLayer.h"
#include <Pine/Core/Base.h>
#include <Pine/World/Scene.h>
#include <Pine/World/Component/Component.h>
#include <Pine/Render/StaticMesh.h>
#include <Pine/Core/FileSystem.h>

#include <filesystem>

#include "ExampleMaterial.h"

using namespace Pine;

ExampleLayer::ExampleLayer()
	: SceneLayer("ExampleLayer")
{
	//Load shader

	FileSystem::MountDir("/SandBox", std::filesystem::current_path().generic_u8string());

	auto pMat = CreateRef<ExampleMaterial>(mShaderLib.Load("ExampleShader", Path("/SandBox/assets/shaders/example.glsl").ToNative()));
	pMat->SetColor(glm::vec4(1,0,0,1));
	MeshUtils::SetDefaultMaterial(pMat);

	//Create Sandbox Scene
	mScene = CreateRef<Pine::Scene>();

	auto meshEntity = mScene->CreateEntity();
	auto& meshComp = meshEntity.AddComponent<StaticMeshComponent>();
	meshComp.StaticMesh = MeshUtils::CreateBox();
	auto& transformComp = meshEntity.GetComponent<TransformComponent>();
	transformComp.Location = glm::vec3(0,0,-5);
}

ExampleLayer::~ExampleLayer()
{
	
}
