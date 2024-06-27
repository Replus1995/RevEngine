#include "pinepch.h"
#include "Input.h"
#include "Pine/Events/KeyEvent.h"
#include "Pine/Events/MouseEvent.h"

namespace Pine
{

static float sMousePositon[2] = {};
static float sMouseScroll[2] = {};
static bool sMouseButtonState[Mouse::Count] = {};
static bool sKeyState[Key::Count] = {};

bool Input::KeyDown(KeyCode key)
{
	return sKeyState[key];
}

bool Input::MouseButtonDown(MouseCode button)
{
	return sMouseButtonState[button];
}

FVector2 Input::GetMousePosition()
{
	return FVector2(sMousePositon[0], sMousePositon[1]);
}

float Input::GetMouseX()
{
	return sMousePositon[0];
}

float Input::GetMouseY()
{
	return sMousePositon[1];
}

void Input::InitState()
{
	memset(sMousePositon, 0, sizeof(sMousePositon));
	memset(sMouseScroll, 0, sizeof(sMouseScroll));
	memset(sMouseButtonState, 0, sizeof(sMouseButtonState));
	memset(sKeyState, 0, sizeof(sKeyState));
}

void Input::UpdateState(Event& e)
{
	switch (e.GetEventType())
	{
	case EventType::KeyPressed:
	{
		KeyEvent* ke = static_cast<KeyEvent*>(&e);
		sKeyState[ke->GetKeyCode()] = true;
		break;
	}
	case EventType::KeyReleased:
	{
		KeyEvent* ke = static_cast<KeyEvent*>(&e);
		sKeyState[ke->GetKeyCode()] = false;
		
	}
	case EventType::MouseButtonPressed:
	{
		MouseButtonEvent* me = static_cast<MouseButtonEvent*>(&e);
		sMouseButtonState[me->GetMouseButton()] = true;
		break;
	}
	case EventType::MouseButtonReleased:
	{
		MouseButtonEvent* me = static_cast<MouseButtonEvent*>(&e);
		sMouseButtonState[me->GetMouseButton()] = false;
		break;
	}
	case EventType::MouseMoved:
	{
		MouseMovedEvent* me = static_cast<MouseMovedEvent*>(&e);
		sMousePositon[0] = me->GetX();
		sMousePositon[1] = me->GetY();
		break;
	}
	case EventType::MouseScrolled:
	{
		MouseScrolledEvent* me = static_cast<MouseScrolledEvent*>(&e);
		sMouseScroll[0] = me->GetXOffset();
		sMouseScroll[1] = me->GetYOffset();
		break;
	}
	default:
		break;
	}
}

}


