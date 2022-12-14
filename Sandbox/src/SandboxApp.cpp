#include <Pine.h>

class SandBox : public Pine::Application
{
public:
	SandBox() {};
	~SandBox() {};
};

Pine::Application* Pine::CreateApplication()
{
	return new SandBox();
}
