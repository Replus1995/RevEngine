#include <Pine.h>

class ExampleLayer : public Pine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{}

	void OnUpdate(float dt) override
	{
		//PE_INFO("ExampleLayer::Update");
	}

	void OnEvent(Pine::Event& e) override
	{
		//PE_TRACE("{0}",e);
	}
};


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
