#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Core/UUID.h"
#include "Pine/Math/Maths.h"
#include "Pine/Math/Transform.h"
#include "Pine/Render/Camera.h"

#include <string>

namespace Pine
{

struct IDComponent
{
	UUID ID;

	IDComponent() = default;
	IDComponent(const IDComponent&) = default;
};

struct TagComponent
{
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag) : Tag(tag) {}
};

struct TransformComponent
{
	FTransform Transform;

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;

	FMatrix4 GetMatrix() const { return Transform.ToMatrix(); }

	FVector3 Location() const { return Transform.Location; }
	FRotator Rotation() const { return Transform.Rotation; }
	FVector3 Scale() const { return Transform.Scale; }

	void SetLocation(const FVector3& t) { Transform.Location = t; }
	void SetRotation(const FRotator& r) { Transform.Rotation = r; }
	void SetScale(const FVector3& s) { Transform.Scale = s; }

};

struct CameraComponent
{
	Camera Camera;
	//bool Primary = true; // TODO: think about moving to Scene
	bool AutoAspectRatio = true;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};

struct StaticMeshComponent
{
	Ref<class StaticMesh> StaticMesh;

	StaticMeshComponent() = default;
	StaticMeshComponent(const StaticMeshComponent& other){ this->StaticMesh = other.StaticMesh; }

};


}