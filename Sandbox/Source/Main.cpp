#include <Rev.h>
#include "ExampleLayer.h"

class SandBoxApp: public Rev::Application
{
public:
	SandBoxApp()
	{
		SetFpsLimit(60);

		//PushLayer(new ExampleLayer());
		//PushLayer(new Rev::ImGuiLayer());
		PushLayer(new Rev::SceneLayer(Rev::CreateRef<Rev::FScene>()));
	};
	~SandBoxApp() {};
};

int main(int argc, char** argv)
{
	return Rev::RunApp<SandBoxApp>(argc, argv);
}