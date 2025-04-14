#pragma once

namespace Engine
{

enum class Key
{
    // Letters
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // Numbers
    NUM0,
    NUM1,
    NUM2,
    NUM3,
    NUM4,
    NUM5,
    NUM6,
    NUM7,
    NUM8,
    NUM9,

    // Numpad
    NUMPAD0,
    NUMPAD1,
    NUMPAD2,
    NUMPAD3,
    NUMPAD4,
    NUMPAD5,
    NUMPAD6,
    NUMPAD7,
    NUMPAD8,
    NUMPAD9,
    NUMPAD_PERIOD,
    NUMPAD_ADD,
    NUMPAD_SUBTRACT,
    NUMPAD_MULTIPLY,
    NUMPAD_DIVIDE,
    NUMPAD_DECIMAL,
    NUMPAD_ENTER,

    // Arrow keys
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,

    // Function row
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,

    // Etc.
	ESCAPE,
	SPACE,
    MINUS,
    EQUAL,
    BACKSPACE,
    TAB,
    ENTER,
    TILDE,
    BACKSLASH,
    SQUARE_BRACKET_LEFT,
    SQUARE_BRACKET_RIGHT,
    SEMICOLON,
    APOSTROPHE,
    COMMA,
    PERIOD,
    SLASH,
    DEL,
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN,
    INSERT,
    NUM_LOCK,
    SCROLL_LOCK,
    CAPS_LOCK,
    PRINT_SCREEN,
    PAUSE,

    // Modifiers
    LEFT_SHIFT,
    LEFT_CONTROL,
    LEFT_ALT,
    LEFT_SUPER,
    RIGHT_SHIFT,
    RIGHT_CONTROL,
    RIGHT_ALT,

    MAX
};


enum class MouseButton
{
    LEFT,
    RIGHT,
    MIDDLE,
    BUTTON4,
    BUTTON5,

    MAX
};


/**
 * @brief Converts a key from the Key enum class to a GLFW key
 * @param key The key you want to convert
 * @return The GLFW key code, GLFW_KEY_UNKNOWN if the key is invalid
 */
[[nodiscard]]
int ConvertKeyGLFW(Engine::Key key);

/**
 * @brief Converts a key from GLFW to a Key enum value
 * @param key The key you want to convert
 * @return The key code, Key::MAX if the key is invalid/not supported
 */
[[nodiscard]]
Engine::Key ConvertGLFWKey(int key);

/**
 * @brief Converts a button from the MouseButton enum class to a GLFW button
 * @param button The button you want to convert
 * @return The GLFW button code, GLFW_MOUSE_BUTTON_LAST if the button is invalid
 */
[[nodiscard]]
int ConvertButtonGLFW(MouseButton button);

/**
 * @brief Converts a key from GLFW to a MouseButton enum value
 * @param button The key you want to convert
 * @return The key code, MouseButton::MAX if the key is invalid/not supported
 */
[[nodiscard]]
MouseButton ConvertGLFWButton(int button);

}