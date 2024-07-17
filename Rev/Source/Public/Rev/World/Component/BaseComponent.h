#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/UUID.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/Camera.h"

#include <string>

namespace Rev
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
	Math::FTransform Transform;

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;

	Math::FMatrix4 GetMatrix() const { return Transform.ToMatrix(); }

	Math::FVector3 Location() const { return Transform.Location; }
	Math::FRotator Rotation() const { return Transform.Rotation; }
	Math::FVector3 Scale() const { return Transform.Scale; }

	void SetLocation(const Math::FVector3& t) { Transform.Location = t; }
	void SetRotation(const Math::FRotator& r) { Transform.Rotation = r; }
	void SetScale(const Math::FVector3& s) { Transform.Scale = s; }

};

struct CameraComponent
{
	Camera Camera;
	//bool Primary = true; // TODO: think about moving to Scene
	bool AutoAspectRatio = true;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};




}