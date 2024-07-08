#include "ExampleLayer.h"
#include <Rev/Core/Base.h>
#include <Rev/World/Scene.h>
#include <Rev/World/Component/Component.h>
#include <Rev/World/System/PlayerCameraSystem.h>
#include <Rev/Render/Renderer.h>
#include <Rev/Render/Mesh/StaticMesh.h>
#include <Rev/Render/Mesh/MeshUtils.h>
#include <Rev/Core/FileSystem.h>

#include <filesystem>

#include "ExampleMaterial.h"

using namespace Rev;

ExampleLayer::ExampleLayer()
	: SceneLayer("ExampleLayer")
{
	//Load shader

	FFileSystem::MountDir("/SandBox", std::filesystem::current_path().generic_u8string());

	Renderer::GetShaderLibrary()->Load("ExampleShader", FPath("/SandBox/assets/shaders/example.glsl").ToNative());

	auto pMat = CreateRef<ExampleMaterial>();
	pMat->SetColor(Math::FLinearColor(.7,.8,.8,1));
	MeshUtils::SetDefaultMaterial(pMat);

	//Create Sandbox Scene
	mScene = CreateRef<Rev::Scene>();

	{
		auto meshEntity = mScene->CreateEntity();
		auto& meshComp = meshEntity.AddComponent<StaticMeshComponent>();
		meshComp.StaticMesh = MeshUtils::CreateBox();
		auto& transformComp = meshEntity.GetComponent<TransformComponent>();
		transformComp.SetLocation(Math::FVector3(0, 0, -5));
	}

	{
		auto camEntity = mScene->CreateEntity();
		auto& camComp = camEntity.AddComponent<CameraComponent>();
		camComp.AutoAspectRatio = true;
		auto& transformComp = camEntity.GetComponent<TransformComponent>();
		transformComp.SetLocation(Math::FVector3(0, 0, 0));
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
