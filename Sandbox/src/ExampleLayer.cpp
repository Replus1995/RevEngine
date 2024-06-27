#include "ExampleLayer.h"
#include <Pine/Core/Base.h>
#include <Pine/World/Scene.h>
#include <Pine/World/Component/Component.h>
#include <Pine/World/System/PlayerCameraSystem.h>
#include <Pine/Render/Renderer.h>
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

	Renderer::GetShaderLibrary()->Load("ExampleShader", Path("/SandBox/assets/shaders/example.glsl").ToNative());

	auto pMat = CreateRef<ExampleMaterial>();
	pMat->SetColor(FVector4(1,0,0,1));
	MeshUtils::SetDefaultMaterial(pMat);

	//Create Sandbox Scene
	mScene = CreateRef<Pine::Scene>();

	{
		auto meshEntity = mScene->CreateEntity();
		auto& meshComp = meshEntity.AddComponent<StaticMeshComponent>();
		meshComp.StaticMesh = MeshUtils::CreateBox();
		auto& transformComp = meshEntity.GetComponent<TransformComponent>();
		transformComp.SetLocation(FVector3(0, 0, -5));
	}

	{
		auto camEntity = mScene->CreateEntity();
		auto& camComp = camEntity.AddComponent<CameraComponent>();
		camComp.AutoAspectRatio = true;
		auto& transformComp = camEntity.GetComponent<TransformComponent>();
		transformComp.SetLocation(FVector3(0, 0, 0));
		//transformComp.SetRotation(FRotator(0, 180, 0));

		auto camSys = mScene->GetSystem<PlayerCameraSystem>();
		if (camSys)
		{
			camSys->SetPlayerCamera(camEntity);
		}
	}
	

}

ExampleLayer::~ExampleLayer()
{
	
}
