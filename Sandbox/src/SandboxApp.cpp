#include <Pine.h>

class ExampleLayer : public Pine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{}

	void OnUpdate() override
	{
		PINE_INFO("ExampleLayer::Update");
	}

	void OnEvent(Pine::Event& e) override
	{
		PINE_TRACE("{0}",e);
	}
};


class SandBox : public Pine::Application
{
public:
	SandBox() 
	{
		PushLayer(new ExampleLayer());
	};
	~SandBox() {};
};

Pine::Application* Pine::CreateApplication()
{
	return new SandBox();
}
