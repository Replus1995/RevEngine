#include <Rev.h>
#include "ExampleLayer.h"
#include "Rev/ImGui/ImGuiLayer.h"
#include "Rev/Render/RenderOptions.h"

class SandBoxApp: public Rev::Application
{
public:
	SandBoxApp()
	{
		SetFpsLimit(60);
		Rev::GRenderOptions.SetNumSamples(4);

		PushLayer(new ExampleLayer());
		//PushLayer(new Rev::SceneLayer(Rev::CreateRef<Rev::FScene>()));
		//PushLayer(new Rev::ImGuiLayer());
	};
	~SandBoxApp() {};
};

int main(int argc, char** argv)
{
	return Rev::RunApp<SandBoxApp>(argc, argv);
}