#include <Pine.h>
#include "ExampleLayer.h"


class SandBox : public Pine::Application
{
public:
	SandBox() 
	{
		PushLayer(new ExampleLayer());
		PushLayer(new Pine::ImGuiLayer());
	};
	~SandBox() {};
};

Pine::Application* Pine::CreateApplication()
{
	return new SandBox();
}

int main(int argc, char** argv)
{
	return Pine::RunApp(argc, argv);
}