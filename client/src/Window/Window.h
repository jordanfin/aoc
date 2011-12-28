#ifndef WINDOW_HANDLE_H
#define WINDOW_HANDLE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Window/VideoModeSupport.h>
#include <Window/Event.h>

////////////////////////////////////////////////////////////
/// Define a low-level window handle type, specific to
/// each platform
////////////////////////////////////////////////////////////
#if defined(SYSTEM_WINDOWS)

    // Windows defines a void* handle (HWND)
    typedef void * WindowHandle;

#elif defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

    // Unix - X11 defines an unsigned integer handle (Window)
    typedef unsigned long WindowHandle;

#elif defined(SYSTEM_MACOS)

    // Mac OS X defines a void * handle (NSWindow)
	typedef void* WindowHandle;

#endif

////////////////////////////////////////////////////////////
/// Structure defining the creation settings of window
////////////////////////////////////////////////////////////
struct WindowSettings
{
    UInt32 DepthBits;         ///< Bits of the depth buffer		(24 by default)
    UInt32 StencilBits;       ///< Bits of the stencil buffer	(8  by default)
    UInt32 AntialiasingLevel; ///< Level of antialiasing		(0  by default)
};

////////////////////////////////////////////////////////////
/// Enumeration of window creation styles
////////////////////////////////////////////////////////////
enum Style
{
	NonStyle   = 0,      ///< No border / title bar (this flag and all others are mutually exclusive)
	Titlebar   = 1 << 0, ///< Title bar + fixed border
	Resize     = 1 << 1, ///< Titlebar + resizable border + maximize button
	Close      = 1 << 2, ///< Titlebar + close button
	Fullscreen = 1 << 3  ///< Fullscreen mode (this flag and all others are mutually exclusive)
};

////////////////////////////////////////////////////////////
/// Window is a rendering window ; it can create a new window
/// or connect to an existing one
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Create (or recreate) the window
///
/// \param Mode :        Video mode to use
/// \param Title :       Title of the window
/// \param WindowStyle : Window style (Resize | Close by default)
/// \param Params :      Creation parameters (see default constructor for default values)
///
////////////////////////////////////////////////////////////
bool WindowCreate(struct VideoMode * Mode, char * Title, unsigned long WindowStyle, struct WindowSettings * Params);

////////////////////////////////////////////////////////////
/// Close (destroy) the window.
/// The Window instance remains valid and you can call
/// Create to recreate the window
///
////////////////////////////////////////////////////////////
void WindowClose();

////////////////////////////////////////////////////////////
/// Enable / disable vertical synchronization
///
/// \param Enabled : True to enable v-sync, false to deactivate
///
////////////////////////////////////////////////////////////
void WindowUseVerticalSync(bool Enabled);

////////////////////////////////////////////////////////////
/// Show or hide the mouse cursor
///
/// \param Show : True to show, false to hide
///
////////////////////////////////////////////////////////////
void WindowShowMouseCursor(bool Show);

////////////////////////////////////////////////////////////
/// Change the position of the mouse cursor
///
/// \param Left : Left coordinate of the cursor, relative to the window
/// \param Top :  Top coordinate of the cursor, relative to the window
///
////////////////////////////////////////////////////////////
void WindowSetCursorPosition(UInt32 Left, UInt32 Top);

////////////////////////////////////////////////////////////
/// Change the position of the window on screen.
/// Only works for top-level windows
///
/// \param Left : Left position
/// \param Top :  Top position
///
////////////////////////////////////////////////////////////
void WindowSetPosition(Int32 Left, Int32 Top);

////////////////////////////////////////////////////////////
/// Change the size of the rendering region of the window
///
/// \param Width :  New width
/// \param Height : New height
///
////////////////////////////////////////////////////////////
void WindowSetSize(UInt32 Width, UInt32 Height);

////////////////////////////////////////////////////////////
/// Show or hide the window
///
/// \param State : True to show, false to hide
///
////////////////////////////////////////////////////////////
void WindowShow(bool State);

////////////////////////////////////////////////////////////
/// Change the window's icon
///
/// \param Width :  Icon's width, in pixels
/// \param Height : Icon's height, in pixels
/// \param Pixels : Pointer to the pixels in memory, format must be RGBA 32 bits
///
////////////////////////////////////////////////////////////
void WindowSetIcon(UInt32 Width, UInt32 Height, UInt8* Pixels);

////////////////////////////////////////////////////////////
/// Activate of deactivate the window as the current target
/// for rendering
///
/// \param Active : True to activate, false to deactivate (true by default)
///
/// \return True if operation was successful, false otherwise
///
////////////////////////////////////////////////////////////
void WindowSetActive(bool Active);

////////////////////////////////////////////////////////////
/// Display the window on screen
///
////////////////////////////////////////////////////////////
void WindowDisplay();

////////////////////////////////////////////////////////////
/// Process incoming events from operating system
///
////////////////////////////////////////////////////////////
void WindowProcessEvents();

////////////////////////////////////////////////////////////
/// Check if there's an active context on the current thread
///
////////////////////////////////////////////////////////////
bool WindowIsContextActive();

////////////////////////////////////////////////////////////
/// Get the event struct from the window
////////////////////////////////////////////////////////////
void WindowGetEvent(struct Event * Copy);

////////////////////////////////////////////////////////////
/// Set the pressed status of a key
////////////////////////////////////////////////////////////
void WindowSetKeyPressed(enum Key Code, bool Pressed);

////////////////////////////////////////////////////////////
/// Get the pressed status of a key
////////////////////////////////////////////////////////////
bool WindowIsKeyPressed(enum Key Code);

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
bool					WindowIsOpened;			///< Check if the window is open or not
bool 					WindowKeyRepeatEnabled;	///< Enable or disable automatic key-repeat.
UInt32					WindowWidth;			///< Window width size
UInt32					WindowHeight;			///< Window height size

#endif // WINDOW_HANDLE_H
