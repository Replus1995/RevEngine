#include <Rev.h>
#include "ExampleLayer.h"


class SandBox : public Rev::Application
{
public:
	SandBox() 
	{
		PushLayer(new ExampleLayer());
		PushLayer(new Rev::ImGuiLayer());
	};
	~SandBox() {};
};

Rev::Application* Rev::CreateApplication()
{
	return new SandBox();
}

int main(int argc, char** argv)
{
	return Rev::RunApp(argc, argv);
}