#pragma once
#include <Pine/World/SceneLayer.h>
#include <Pine/Render/Resource/Shader.h>

class ExampleLayer : public Pine::SceneLayer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

private:
	Pine::ShaderLibrary mShaderLib;

};