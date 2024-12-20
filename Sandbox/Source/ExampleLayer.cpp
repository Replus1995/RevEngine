#include "ExampleLayer.h"
#include <Rev/Core/Base.h>
#include <Rev/World/Scene.h>
#include <Rev/World/Component/AllComponents.h>
#include <Rev/World/System/PlayerCameraSystem.h>
#include <Rev/Render/Material/PBRMaterial.h>
#include <Rev/Render/RHI/RHIShaderLibrary.h>
#include <Rev/Core/FileSystem.h>
#include <Rev/Asset/AssetLibrary.h>
#include <Rev/Asset/TextureStorage.h>

#include <filesystem>

#include "ExampleMaterial.h"

using namespace Rev;

ExampleLayer::ExampleLayer()
	: SceneLayer("ExampleLayer")
{
	//Load shader

	//FFileSystem::MountDir("/SandBox", std::filesystem::current_path().generic_u8string());
	FFileSystem::MountDir("/Game", (std::filesystem::current_path() / "Game").generic_string());

	//Create Sandbox Scene
	mScene = CreateRef<Rev::FScene>();

	//FAssetLibrary::GetDefaultSurfaceMaterial();

	/*{
		auto pBoxMat = CreateRef<ExampleMaterial>();
		pBoxMat->Compile();
		pBoxMat->SetColor(Math::FLinearColor(.7, .9, .8, 1));

		auto meshEntity = mScene->CreateEntity();
		auto& meshComp = meshEntity.AddComponent<StaticMeshComponent>();
		meshComp.StaticMesh = FAssetLibrary::CreateBasicGeometry(EBasicGeometry::Box, pBoxMat);
		auto& transformComp = meshEntity.GetComponent<TransformComponent>();
		transformComp.SetLocation(Math::FVector3(0, 0.6, -5));
	}*/

	{
		//auto importRes = FAssetLibrary::ImportMesh(FPath("/Game/Assets/Models/Cube/Cube.gltf"));
		auto importRes = FAssetLibrary::ImportModel(FPath("/Game/Assets/Models/DamagedHelmet.glb"));
		if (!importRes.StaticMeshes.empty())
		{
			auto meshEntity = mScene->CreateEntity();
			auto& meshComp = meshEntity.AddComponent<StaticMeshComponent>();
			meshComp.StaticMesh = importRes.StaticMeshes[0];
			auto & transformComp = meshEntity.GetComponent<TransformComponent>();
			transformComp.SetLocation(Math::FVector3(0, 0.6, -5));
			transformComp.SetScale(Math::FVector3(0.5, 0.5, 0.5));
			transformComp.SetRotation(Math::FRotator(90, 0, 0));
		}
		
	}

	{
		auto pPlaneMat = CreateRef<FPBRMaterial>();
		pPlaneMat->Compile();
		//pPlaneMat->PBRMaterialParams.BaseColorFactor = (Math::FLinearColor(.8, .8, .8, 1));
		pPlaneMat->PBRMaterialParams.Metallic = 0.3f;
		pPlaneMat->PBRMaterialParams.Roughness = 0.8f;

		auto meshEntity = mScene->CreateEntity();
		auto& meshComp = meshEntity.AddComponent<StaticMeshComponent>();
		meshComp.StaticMesh = FAssetLibrary::CreateBasicGeometry(EBasicGeometry::Plane, pPlaneMat);
		auto& transformComp = meshEntity.GetComponent<TransformComponent>();
		transformComp.SetLocation(Math::FVector3(0, 0, -5));
		transformComp.SetRotation(Math::FRotator(-90.0F, 0, 0));
		transformComp.SetScale(Math::FVector3(5, 5, 0));
	}

	{
		auto lightEntity = mScene->CreateEntity();
		auto& dirLightComp = lightEntity.AddComponent<DirectionalLightComponent>();
		dirLightComp.Light.SetColorByTemperature(6500.0f);
		dirLightComp.Light.SetIntensity(8.0f);
		auto& transformComp = lightEntity.GetComponent<TransformComponent>();
		transformComp.SetLocation(Math::FVector3(0, 0, 0));
		transformComp.SetRotation(Math::FRotator(0, 0, 45.0f));
	}

	{
		auto camEntity = mScene->CreateEntity();
		auto& camComp = camEntity.AddComponent<CameraComponent>();
		camComp.AutoAspectRatio = true;
		//camComp.Camera.SetPerspective(30.0f, 0.01f, 10000.0f);
		auto& transformComp = camEntity.GetComponent<TransformComponent>();
		transformComp.SetLocation(Math::FVector3(0, 2, 0));
		transformComp.SetRotation(Math::FRotator(-30.0f, 0, 0));

		auto camSys = mScene->GetSystem<PlayerCameraSystem>();
		if (camSys)
		{
			camSys->SetPlayerCamera(camEntity);
		}
	}

	{
		//CubeMap
		auto CubeTexStorage = FAssetLibrary::ImportTextureCube(
			"/Game/Assets/Textures/Skybox/right.jpg", 
			"/Game/Assets/Textures/Skybox/left.jpg",
			"/Game/Assets/Textures/Skybox/top.jpg",
			"/Game/Assets/Textures/Skybox/bottom.jpg",
			"/Game/Assets/Textures/Skybox/front.jpg",
			"/Game/Assets/Textures/Skybox/back.jpg",
			true);
		auto CubeTex = CubeTexStorage.CreateTexture();
		if (CubeTex)
		{ 
			//Enviornment
			auto  skyEntity = mScene->CreateEntity();
			auto& skyComp = skyEntity.AddComponent<SkyComponent>();
			skyComp.Skybox.SetEnvironmentTexture(CubeTex);
		}
	}
}

ExampleLayer::~ExampleLayer()
{
}
