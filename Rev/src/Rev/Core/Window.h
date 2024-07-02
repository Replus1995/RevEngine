#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Events/Event.h"

namespace Pine
{

struct WindowProps
{
	std::string Title;
	unsigned int Width;
	unsigned int Height;

	WindowProps(const std::string& title = "Pine Engine",
				unsigned int width = 1280,
				unsigned int height = 720)
		: Title(title), Width(width), Height(height)
	{
	}
};

enum class EWindowType : uint8_t
{
	UNKNOWN = 0,
	GLFW = 1
};


class PINE_API Window
{
public:
	using EventCallbackFn = std::function<void(Event&)>;

	virtual ~Window() {}

	virtual void OnUpdate() = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	//Window attributes
	virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;

	virtual void SetClipboardText(const char* text) = 0;
	virtual const char* GetClipboardText() = 0;

	EWindowType GetType() const { return mType; }

	static Scope<Window> Create(const WindowProps& props = WindowProps());

protected:
	EWindowType mType = EWindowType::UNKNOWN;
};

}