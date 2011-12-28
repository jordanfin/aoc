#ifndef WINDOW_EVENT_H
#define WINDOW_EVENT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
/// Definition of key codes for keyboard events
////////////////////////////////////////////////////////////
enum KeyCode
{
	A = 'a',
	B = 'b',
	C = 'c',
	D = 'd',
	E = 'e',
	F = 'f',
	G = 'g',
	H = 'h',
	I = 'i',
	J = 'j',
	K = 'k',
	L = 'l',
	M = 'm',
	N = 'n',
	O = 'o',
	P = 'p',
	Q = 'q',
	R = 'r',
	S = 's',
	T = 't',
	U = 'u',
	V = 'v',
	W = 'w',
	X = 'x',
	Y = 'y',
	Z = 'z',
	Num0 = '0',
	Num1 = '1',
	Num2 = '2',
	Num3 = '3',
	Num4 = '4',
	Num5 = '5',
	Num6 = '6',
	Num7 = '7',
	Num8 = '8',
	Num9 = '9',
	Esc = 256,
	LControl,
	LShift,
	LAlt,
	LSystem,      ///< OS specific key (left side) : windows (Win and Linux), apple (MacOS), ...
	RControl,
	RShift,
	RAlt,
	RSystem,      ///< OS specific key (right side) : windows (Win and Linux), apple (MacOS), ...
	Menu,
	LBracket,     ///< [
	RBracket,     ///< ]
	SemiColon,    ///< ;
	Comma,        ///< ,
	Period,       ///< .
	Quote,        ///< '
	Slash,        ///< /
	BackSlash,
	Tilde,        ///< ~
	Equal,        ///< =
	Dash,         ///< -
	Space,
	Return,
	Back,
	Tab,
	PageUp,
	PageDown,
	End,
	Home,
	Insert,
	Delete,
	Add,          ///< +
	Subtract,     ///< -
	Multiply,     ///< *
	Divide,       ///< /
	Left,         ///< Left arrow
	Right,        ///< Right arrow
	Up,           ///< Up arrow
	Down,         ///< Down arrow
	Numpad0,
	Numpad1,
	Numpad2,
	Numpad3,
	Numpad4,
	Numpad5,
	Numpad6,
	Numpad7,
	Numpad8,
	Numpad9,
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
	Pause,
	KeyCodeCount
};


////////////////////////////////////////////////////////////
/// Definition of button codes for mouse events
////////////////////////////////////////////////////////////
enum MouseButton
{
	LeftButton,
	RightButton,
	Middle,
	XButton1,
	XButton2
};

////////////////////////////////////////////////////////////
/// Event defines a system event and its parameters
////////////////////////////////////////////////////////////
struct Event
{
    ////////////////////////////////////////////////////////////
    /// Keyboard event parameters
    ////////////////////////////////////////////////////////////
    struct KeyEvent
    {
        enum KeyCode	Code;
        bool      		Alt;
        bool			Control;
        bool			Shift;
    } Key;

    ////////////////////////////////////////////////////////////
    /// Text event parameters
    ////////////////////////////////////////////////////////////
    struct TextEvent
    {
        UInt32 Unicode;
    } Text;

    ////////////////////////////////////////////////////////////
    /// Mouse move event parameters
    ////////////////////////////////////////////////////////////
    struct MouseMoveEvent
    {
        Int32 X;
        Int32 Y;
    } MouseMove;

    ////////////////////////////////////////////////////////////
    /// Mouse buttons events parameters
    ////////////////////////////////////////////////////////////
    struct MouseButtonEvent
    {
        enum MouseButton	Button;
        Int32				X;
        Int32				Y;
	} MouseButton;

    ////////////////////////////////////////////////////////////
    /// Mouse wheel events parameters
    ////////////////////////////////////////////////////////////
    struct MouseWheelEvent
    {
        Int32 Delta;
    } MouseWheel;

    ////////////////////////////////////////////////////////////
    /// Size events parameters
    ////////////////////////////////////////////////////////////
    struct SizeEvent
    {
        UInt32 Width;
        UInt32 Height;
    } Size;

    ////////////////////////////////////////////////////////////
    /// Enumeration of the different types of events
    ////////////////////////////////////////////////////////////
    enum EventType
    {
        Closed = 1,
        Resized,
        LostFocus,
        GainedFocus,
        TextEntered,
        KeyPressed,
        KeyReleased,
        MouseWheelMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseEntered,
        MouseLeft
    } Type;
};

#endif // WINDOW_EVENT_H
