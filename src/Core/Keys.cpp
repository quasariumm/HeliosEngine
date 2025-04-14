#include "Keys.h"

#include <GLFW/glfw3.h>


namespace Engine
{

int ConvertGLFW(const Key key)
{
	switch (key)
	{
	case Key::A:
		return GLFW_KEY_A;
	case Key::B:
		return GLFW_KEY_B;
	case Key::C:
		return GLFW_KEY_C;
	case Key::D:
		return GLFW_KEY_D;
	case Key::E:
		return GLFW_KEY_E;
	case Key::F:
		return GLFW_KEY_F;
	case Key::G:
		return GLFW_KEY_G;
	case Key::H:
		return GLFW_KEY_H;
	case Key::I:
		return GLFW_KEY_I;
	case Key::J:
		return GLFW_KEY_J;
	case Key::K:
		return GLFW_KEY_K;
	case Key::L:
		return GLFW_KEY_L;
	case Key::M:
		return GLFW_KEY_M;
	case Key::N:
		return GLFW_KEY_N;
	case Key::O:
		return GLFW_KEY_O;
	case Key::P:
		return GLFW_KEY_P;
	case Key::Q:
		return GLFW_KEY_Q;
	case Key::R:
		return GLFW_KEY_R;
	case Key::S:
		return GLFW_KEY_S;
	case Key::T:
		return GLFW_KEY_T;
	case Key::U:
		return GLFW_KEY_U;
	case Key::V:
		return GLFW_KEY_V;
	case Key::W:
		return GLFW_KEY_W;
	case Key::X:
		return GLFW_KEY_X;
	case Key::Y:
		return GLFW_KEY_Y;
	case Key::Z:
		return GLFW_KEY_Z;

	case Key::NUM0:
		return GLFW_KEY_0;
	case Key::NUM1:
		return GLFW_KEY_1;
	case Key::NUM2:
		return GLFW_KEY_2;
	case Key::NUM3:
		return GLFW_KEY_3;
	case Key::NUM4:
		return GLFW_KEY_4;
	case Key::NUM5:
		return GLFW_KEY_5;
	case Key::NUM6:
		return GLFW_KEY_6;
	case Key::NUM7:
		return GLFW_KEY_7;
	case Key::NUM8:
		return GLFW_KEY_8;
	case Key::NUM9:
		return GLFW_KEY_9;

	case Key::NUMPAD0:
		return GLFW_KEY_KP_0;
	case Key::NUMPAD1:
		return GLFW_KEY_KP_1;
	case Key::NUMPAD2:
		return GLFW_KEY_KP_2;
	case Key::NUMPAD3:
		return GLFW_KEY_KP_3;
	case Key::NUMPAD4:
		return GLFW_KEY_KP_4;
	case Key::NUMPAD5:
		return GLFW_KEY_KP_5;
	case Key::NUMPAD6:
		return GLFW_KEY_KP_6;
	case Key::NUMPAD7:
		return GLFW_KEY_KP_7;
	case Key::NUMPAD8:
		return GLFW_KEY_KP_8;
	case Key::NUMPAD9:
		return GLFW_KEY_KP_9;
	case Key::NUMPAD_PERIOD:
		return GLFW_KEY_KP_DECIMAL;
	case Key::NUMPAD_ADD:
		return GLFW_KEY_KP_ADD;
	case Key::NUMPAD_SUBTRACT:
		return GLFW_KEY_KP_SUBTRACT;
	case Key::NUMPAD_MULTIPLY:
		return GLFW_KEY_KP_MULTIPLY;
	case Key::NUMPAD_DECIMAL:
		return GLFW_KEY_KP_DECIMAL;
	case Key::NUMPAD_ENTER:
		return GLFW_KEY_KP_ENTER;

	case Key::ARROW_LEFT:
		return GLFW_KEY_LEFT;
	case Key::ARROW_RIGHT:
		return GLFW_KEY_RIGHT;
	case Key::ARROW_UP:
		return GLFW_KEY_UP;
	case Key::ARROW_DOWN:
		return GLFW_KEY_DOWN;

	case Key::F1:
		return GLFW_KEY_F1;
	case Key::F2:
		return GLFW_KEY_F2;
	case Key::F3:
		return GLFW_KEY_F3;
	case Key::F4:
		return GLFW_KEY_F4;
	case Key::F5:
		return GLFW_KEY_F5;
	case Key::F6:
		return GLFW_KEY_F6;
	case Key::F7:
		return GLFW_KEY_F7;
	case Key::F8:
		return GLFW_KEY_F8;
	case Key::F9:
		return GLFW_KEY_F9;
	case Key::F10:
		return GLFW_KEY_F10;
	case Key::F11:
		return GLFW_KEY_F11;
	case Key::F12:
		return GLFW_KEY_F12;
	case Key::F13:
		return GLFW_KEY_F13;
	case Key::F14:
		return GLFW_KEY_F14;
	case Key::F15:
		return GLFW_KEY_F15;
	case Key::F16:
		return GLFW_KEY_F16;
	case Key::F17:
		return GLFW_KEY_F17;
	case Key::F18:
		return GLFW_KEY_F18;
	case Key::F19:
		return GLFW_KEY_F19;
	case Key::F20:
		return GLFW_KEY_F20;
	case Key::F21:
		return GLFW_KEY_F21;
	case Key::F22:
		return GLFW_KEY_F22;
	case Key::F23:
		return GLFW_KEY_F23;
	case Key::F24:
		return GLFW_KEY_F24;

	case Key::MINUS:
		return GLFW_KEY_MINUS;
	case Key::EQUAL:
		return GLFW_KEY_EQUAL;
	case Key::BACKSPACE:
		return GLFW_KEY_BACKSPACE;
	case Key::TAB:
		return GLFW_KEY_TAB;
	case Key::ENTER:
		return GLFW_KEY_ENTER;
	case Key::TILDE:
		return GLFW_KEY_GRAVE_ACCENT;
	case Key::BACKSLASH:
		return GLFW_KEY_BACKSLASH;
	case Key::SQUARE_BRACKET_LEFT:
		return GLFW_KEY_LEFT_BRACKET;
	case Key::SQUARE_BRACKET_RIGHT:
		return GLFW_KEY_RIGHT_BRACKET;
	case Key::SEMICOLON:
		return GLFW_KEY_SEMICOLON;
	case Key::APOSTROPHE:
		return GLFW_KEY_APOSTROPHE;
	case Key::COMMA:
		return GLFW_KEY_COMMA;
	case Key::PERIOD:
		return GLFW_KEY_PERIOD;
	case Key::SLASH:
		return GLFW_KEY_SLASH;

	case Key::DELETE:
		return GLFW_KEY_DELETE;
	case Key::HOME:
		return GLFW_KEY_HOME;
	case Key::END:
		return GLFW_KEY_END;
	case Key::PAGE_UP:
		return GLFW_KEY_PAGE_UP;
	case Key::PAGE_DOWN:
		return GLFW_KEY_PAGE_DOWN;
	case Key::INSERT:
		return GLFW_KEY_INSERT;

	case Key::NUM_LOCK:
		return GLFW_KEY_NUM_LOCK;
	case Key::SCROLL_LOCK:
		return GLFW_KEY_SCROLL_LOCK;
	case Key::CAPS_LOCK:
		return GLFW_KEY_CAPS_LOCK;
	case Key::PRINT_SCREEN:
		return GLFW_KEY_PRINT_SCREEN;
	case Key::PAUSE:
		return GLFW_KEY_PAUSE;

	case Key::LEFT_SHIFT:
		return GLFW_KEY_LEFT_SHIFT;
	case Key::LEFT_CONTROL:
		return GLFW_KEY_LEFT_CONTROL;
	case Key::LEFT_ALT:
		return GLFW_KEY_LEFT_ALT;
	case Key::LEFT_SUPER:
		return GLFW_KEY_LEFT_SUPER;
	case Key::RIGHT_SHIFT:
		return GLFW_KEY_RIGHT_SHIFT;
	case Key::RIGHT_CONTROL:
		return GLFW_KEY_RIGHT_CONTROL;
	case Key::RIGHT_ALT:
		return GLFW_KEY_RIGHT_ALT;

	default:
		return GLFW_KEY_UNKNOWN;
	}
}


int ConvertGLFW(const MouseButton button)
{
	switch (button)
	{
	case MouseButton::LEFT:
		return GLFW_MOUSE_BUTTON_LEFT;
	case MouseButton::RIGHT:
		return GLFW_MOUSE_BUTTON_RIGHT;
	case MouseButton::MIDDLE:
		return GLFW_MOUSE_BUTTON_MIDDLE;
	case MouseButton::BUTTON4:
		return GLFW_MOUSE_BUTTON_4;
	case MouseButton::BUTTON5:
		return GLFW_MOUSE_BUTTON_5;
	default:
		return GLFW_MOUSE_BUTTON_LAST;
	}
}

}
