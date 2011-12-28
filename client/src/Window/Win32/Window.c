////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#define _WIN32_WINDOWS 0x0501
#define _WIN32_WINNT   0x0501
#include <Window/Window.h>
#include <Graphics/OpenGLHelper.h>
#include <System/Mutex.h>
#include <System/Error.h>

// Old versions of MinGW lack the definition of XBUTTON1 and XBUTTON2
#ifndef XBUTTON1
    #define XBUTTON1 0x0001
#endif
#ifndef XBUTTON2
    #define XBUTTON2 0x0002
#endif

////////////////////////////////////////////////////////////
// Static member data
////////////////////////////////////////////////////////////
const char*		WindowClassNameA	= "hWndClsA";	///< Win32 window class name (ANSI version)
const wchar_t*	WindowClassNameW	= L"hWndClsW";	///< Win32 window class name (unicode version)

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
HWND         WindowhWnd				= NULL;		///< Win32 handle of the window
long         WindowCallback			= 0;		///< Stores the original event callback function of the control
HCURSOR      WindowCursor 			= NULL;		///< The system cursor to display into the window
HICON        WindowIcon 			= NULL;		///< Custom icon assigned to the window
bool         WindowIsCursorIn 		= false;	///< Is the mouse cursor in the window's area ?
HDC          WindowDeviceContext;				///< HDC associated to the window
HGLRC        WindowGLContext;					///< OpenGL rendering context associated to the HDC
struct Event WindowEvent			= {{0}};	///< Window input
Mutex        WindowEventMutex;					///< Event input mutex
bool		 EventUpdated			= false;	///< Check if the event was updated
bool		 KeyArray[KeyCodeCount] = {{0}};	///< Array to check pressed keys

////////////////////////////////////////////////////////////
// Internal methods
////////////////////////////////////////////////////////////
static LRESULT CALLBACK	GlobalOnEvent(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam); ///< Function called whenever one of our windows receives a message
void 					RegisterWindowClass(); 													///< Register the window class
bool 					HasUnicodeSupport(); 													///< Check if the current version of the OS supports unicode messages and functions
void					ProcessEvent(UINT Message, WPARAM WParam, LPARAM LParam);				///< Process a Win32 event
void					SwitchToFullscreen(struct VideoMode * Mode);							///< Switch to fullscreen mode
void					CreateContext(struct VideoMode * Mode, struct WindowSettings * Params); ///< Construct the context from graphics settings
void					Cleanup();																///< Free all the graphical resources attached to the window
enum KeyCode			GetShiftState(bool KeyDown);											///< Check the state of the shift keys on a key event, and return the corresponding KeyCode
enum KeyCode			VirtualKeyCode(WPARAM VirtualKey, LPARAM Flags);						///< Convert a Win32 virtual key code to a KeyCode
int						EvaluateConfig(struct VideoMode * Mode, struct WindowSettings * Settings, int ColorBits, int DepthBits, int StencilBits, int Antialiasing);

////////////////////////////////////////////////////////////
/// Create (or recreate) the window
////////////////////////////////////////////////////////////
bool WindowCreate(struct VideoMode * Mode, char * Title, unsigned long WindowStyle, struct WindowSettings * Params)
{
	int Left, Top, Width, Height;
	bool FullScreen = false;
	DWORD Win32Style;
	RECT ActualRect;

    // Initialize the event mutex
    MutexInitialize(&WindowEventMutex);

    // Register the window class at first call
    RegisterWindowClass();

    // Compute position and size
    Left   = (GetDeviceCaps(GetDC(NULL), HORZRES) - Mode->Width)  / 2;
    Top    = (GetDeviceCaps(GetDC(NULL), VERTRES) - Mode->Height) / 2;
    Width  = WindowWidth  = Mode->Width;
    Height = WindowHeight = Mode->Height;

    // Choose the window style according to the Style parameter
    Win32Style = WS_VISIBLE;
    if (WindowStyle == NonStyle)
    {
        Win32Style |= WS_POPUP;
    }
    else
    {
        if (WindowStyle & Titlebar) Win32Style |= WS_CAPTION | WS_MINIMIZEBOX;
        if (WindowStyle & Resize)   Win32Style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
        if (WindowStyle & Close)    Win32Style |= WS_SYSMENU;
    }

    // In windowed mode, adjust width and height so that window will have the requested client area
    FullScreen = ((WindowStyle & Fullscreen) != 0);
    if (!FullScreen)
    {
        RECT Rect = {0, 0, Width, Height};
        AdjustWindowRect(&Rect, Win32Style, false);
        Width  = Rect.right - Rect.left;
        Height = Rect.bottom - Rect.top;
    }

    // Create the window
    if (HasUnicodeSupport())
    {
        wchar_t WTitle[256];
        int NbChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)(Title), (int)(strlen(Title)), WTitle, sizeof(WTitle) / sizeof(*WTitle));
        WTitle[NbChars] = L'\0';
        WindowhWnd = CreateWindowW(WindowClassNameW, WTitle, Win32Style, Left, Top, Width, Height, NULL, NULL, GetModuleHandle(NULL), NULL);
    }
    else
    {
        WindowhWnd = CreateWindowA(WindowClassNameA, (LPCSTR)(Title), Win32Style, Left, Top, Width, Height, NULL, NULL, GetModuleHandle(NULL), NULL);
    }

    // Switch to fullscreen if requested
    if (FullScreen)
        SwitchToFullscreen(Mode);

    // Create the rendering context
    if (WindowhWnd)
        CreateContext(Mode, Params);

    // Get the actual size of the window, which can be smaller even after the call to AdjustWindowRect
    // This happens when the window is bigger than the desktop
    GetClientRect(WindowhWnd, &ActualRect);
    WindowWidth  = ActualRect.right - ActualRect.left;
    WindowHeight = ActualRect.bottom - ActualRect.top;

    // Set window open state to true
    WindowIsOpened = true;

    // Key Repeat is enabled
    WindowKeyRepeatEnabled = false;

	// Resize the OpenGL scene
	OpenGLResizeScene();

	return true;
}


////////////////////////////////////////////////////////////
/// Close (destroy) the window.
////////////////////////////////////////////////////////////
void WindowClose()
{
    // Destroy the custom icon, if any
    if (WindowIcon)
        DestroyIcon(WindowIcon);

    if (!WindowCallback)
    {
        // Destroy the window
        if (WindowhWnd)
            DestroyWindow(WindowhWnd);

        // Unregister window class if we were the last window
		if (HasUnicodeSupport())
		{
			UnregisterClassW(WindowClassNameW, GetModuleHandle(NULL));
		}
		else
		{
			UnregisterClassA(WindowClassNameA, GetModuleHandle(NULL));
		}
    }
    else
    {
        // The window is external : remove the hook on its message callback
        SetWindowLongPtr(WindowhWnd, GWLP_WNDPROC, WindowCallback);
    }

	// Set window open state to false
	WindowIsOpened = false;

    // Destroy the event mutex
    MutexDestroy(&WindowEventMutex);
}

////////////////////////////////////////////////////////////
/// Check if there's an active context on the current thread
////////////////////////////////////////////////////////////
bool WindowIsContextActive()
{
    return wglGetCurrentContext() != NULL;
}

////////////////////////////////////////////////////////////
/// Process incoming events from operating system
////////////////////////////////////////////////////////////
void WindowProcessEvents()
{
    // We update the window only if we own it
    if (!WindowCallback)
    {
        MSG Message;
        while (PeekMessage(&Message, WindowhWnd, 0, 0, PM_REMOVE))
        {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
        }
    }
}


////////////////////////////////////////////////////////////
/// Display the window on screen
////////////////////////////////////////////////////////////
void WindowDisplay()
{
    if (WindowDeviceContext && WindowGLContext)
        SwapBuffers(WindowDeviceContext);
}


////////////////////////////////////////////////////////////
/// Activate of deactivate the window as the current target
/// for rendering
////////////////////////////////////////////////////////////
void WindowSetActive(bool Active)
{
    if (Active)
    {
        if (WindowDeviceContext && WindowGLContext && (wglGetCurrentContext() != WindowGLContext))
            wglMakeCurrent(WindowDeviceContext, WindowGLContext);
    }
    else
    {
        if (wglGetCurrentContext() == WindowGLContext)
            wglMakeCurrent(NULL, NULL);
    }
}


////////////////////////////////////////////////////////////
/// Enable / disable vertical synchronization
////////////////////////////////////////////////////////////
void WindowUseVerticalSync(bool Enabled)
{
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)(wglGetProcAddress("wglSwapIntervalEXT"));
    if (wglSwapIntervalEXT)
        wglSwapIntervalEXT(Enabled ? 1 : 0);
}


////////////////////////////////////////////////////////////
/// Show or hide the mouse cursor
////////////////////////////////////////////////////////////
void WindowShowMouseCursor(bool Show)
{
    if (Show)
        WindowCursor = LoadCursor(NULL, IDC_ARROW);
    else
        WindowCursor = NULL;

    SetCursor(WindowCursor);
}


////////////////////////////////////////////////////////////
/// Change the position of the mouse cursor
////////////////////////////////////////////////////////////
void WindowSetCursorPosition(UInt32 Left, UInt32 Top)
{
    POINT Pos = {Left, Top};
    ClientToScreen(WindowhWnd, &Pos);
    SetCursorPos(Pos.x, Pos.y);
}


////////////////////////////////////////////////////////////
/// Change the position of the window on screen.
////////////////////////////////////////////////////////////
void WindowSetPosition(Int32 Left, Int32 Top)
{
    SetWindowPos(WindowhWnd, NULL, Left, Top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


////////////////////////////////////////////////////////////
/// Change the size of the window
////////////////////////////////////////////////////////////
void WindowSetSize(UInt32 Width, UInt32 Height)
{
    // SetWindowPos wants the total size of the window (including title bar and borders),
    // so we have to compute it
    RECT Rect = {0, 0, Width, Height};
    AdjustWindowRect(&Rect, GetWindowLong(WindowhWnd, GWL_STYLE), false);
    Width  = Rect.right - Rect.left;
    Height = Rect.bottom - Rect.top;

    SetWindowPos(WindowhWnd, NULL, 0, 0, Width, Height, SWP_NOMOVE | SWP_NOZORDER);
}

////////////////////////////////////////////////////////////
/// Show or hide the window
////////////////////////////////////////////////////////////
void WindowShow(bool State)
{
    ShowWindow(WindowhWnd, State ? SW_SHOW : SW_HIDE);
}

////////////////////////////////////////////////////////////
/// Change the window's icon
////////////////////////////////////////////////////////////
void WindowSetIcon(UInt32 Width, UInt32 Height, UInt8* Pixels)
{
	Int32 i;
	UInt8 * IconPixels;

    // First destroy the previous one
    if (WindowIcon)
        DestroyIcon(WindowIcon);

	IconPixels = (UInt8*)malloc(Width * Height * 4);

    // Windows wants BGRA pixels : swap red and blue channels
    for (i = 0; i < sizeof(IconPixels) / 4; ++i)
    {
        IconPixels[i * 4 + 0] = Pixels[i * 4 + 2];
        IconPixels[i * 4 + 1] = Pixels[i * 4 + 1];
        IconPixels[i * 4 + 2] = Pixels[i * 4 + 0];
        IconPixels[i * 4 + 3] = Pixels[i * 4 + 3];
    }

    // Create the icon from the pixels array
    WindowIcon = CreateIcon(GetModuleHandle(NULL), Width, Height, 1, 32, NULL, &IconPixels[0]);

    // Set it as both big and small icon of the window
    if (WindowIcon)
    {
        SendMessage(WindowhWnd, WM_SETICON, ICON_BIG,   (LPARAM)WindowIcon);
        SendMessage(WindowhWnd, WM_SETICON, ICON_SMALL, (LPARAM)WindowIcon);
    }
    else
    {
        MessageError("WindowSetIcon", "Failed to set the window's icon.");
    }

	free(IconPixels);
}


////////////////////////////////////////////////////////////
/// Register the window class
////////////////////////////////////////////////////////////
void RegisterWindowClass()
{
    if (HasUnicodeSupport())
    {
        WNDCLASSW WindowClass;
        WindowClass.style         = 0;
        WindowClass.lpfnWndProc   = &GlobalOnEvent;
        WindowClass.cbClsExtra    = 0;
        WindowClass.cbWndExtra    = 0;
        WindowClass.hInstance     = GetModuleHandle(NULL);
        WindowClass.hIcon         = NULL;
        WindowClass.hCursor       = 0;
        WindowClass.hbrBackground = 0;
        WindowClass.lpszMenuName  = NULL;
        WindowClass.lpszClassName = WindowClassNameW;
        RegisterClassW(&WindowClass);
    }
    else
    {
        WNDCLASSA WindowClass;
        WindowClass.style         = 0;
        WindowClass.lpfnWndProc   = &GlobalOnEvent;
        WindowClass.cbClsExtra    = 0;
        WindowClass.cbWndExtra    = 0;
        WindowClass.hInstance     = GetModuleHandle(NULL);
        WindowClass.hIcon         = NULL;
        WindowClass.hCursor       = 0;
        WindowClass.hbrBackground = 0;
        WindowClass.lpszMenuName  = NULL;
        WindowClass.lpszClassName = WindowClassNameA;
        RegisterClassA(&WindowClass);
    }
}


////////////////////////////////////////////////////////////
/// Switch to fullscreen mode
////////////////////////////////////////////////////////////
void SwitchToFullscreen(struct VideoMode * Mode)
{
    DEVMODE DevMode;
	long Style;

    DevMode.dmSize       = sizeof(DEVMODE);
    DevMode.dmPelsWidth  = Mode->Width;
    DevMode.dmPelsHeight = Mode->Height;
    DevMode.dmBitsPerPel = Mode->BitsPerPixel;
    DevMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

    // Apply fullscreen mode
    if (ChangeDisplaySettings(&DevMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
        MessageError("SwitchToFullscreen", "Failed to change display mode for fullscreen.");
        return;
    }

    // Change window style (no border, no titlebar, ...)
    SetWindowLong(WindowhWnd, GWL_STYLE,   WS_POPUP);
    SetWindowLong(WindowhWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);

    // And resize it so that it fits the entire screen
    SetWindowPos(WindowhWnd, HWND_TOP, 0, 0, Mode->Width, Mode->Height, SWP_FRAMECHANGED);
    ShowWindow(WindowhWnd, SW_SHOW);

    // SetPixelFormat can fail (really ?) if window style doesn't contain these flags
    Style = GetWindowLong(WindowhWnd, GWL_STYLE);
    SetWindowLong(WindowhWnd, GWL_STYLE, Style | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
}


////////////////////////////////////////////////////////////
/// Construct the context from graphics settings
////////////////////////////////////////////////////////////
void CreateContext(struct VideoMode * Mode, struct WindowSettings * Params)
{
	int BestFormat, Color, Score;
	PIXELFORMATDESCRIPTOR ActualFormat;
	HGLRC CurrentContext;

    // Get the device context attached to the window
    WindowDeviceContext = GetDC(WindowhWnd);
    if (WindowDeviceContext == NULL)
    {
        MessageError("CreateContext", "Failed to get device context of window -- cannot create OpenGL context.");
        return;
    }

    // Let's find a suitable pixel format -- first try with antialiasing
    BestFormat = 0;
    if (Params->AntialiasingLevel > 0)
    {
        // Get the wglChoosePixelFormatARB function (it is an extension)
        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB"));

        // Define the basic attributes we want for our window
        int IntAttributes[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		    WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
		    WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_SAMPLE_BUFFERS_ARB, (Params->AntialiasingLevel ? GL_TRUE : GL_FALSE),
		    WGL_SAMPLES_ARB,        Params->AntialiasingLevel,
		    0,                      0
        };

        // Let's check how many formats are supporting our requirements
        int   Formats[128];
	    UINT  NbFormats;
	    float FloatAttributes[] = {0, 0};
	    bool  IsValid = wglChoosePixelFormatARB(WindowDeviceContext, IntAttributes, FloatAttributes, sizeof(Formats) / sizeof(*Formats), Formats, &NbFormats) != 0;
        if (!IsValid || (NbFormats == 0))
        {
            if (Params->AntialiasingLevel > 2)
            {
                // No format matching our needs : reduce the multisampling level
				MessageError("CreateContext", "Failed to find a pixel format supporting %u antialiasing levels ; trying with 2 levels.", Params->AntialiasingLevel);

                Params->AntialiasingLevel = IntAttributes[1] = 2;
	            IsValid = wglChoosePixelFormatARB(WindowDeviceContext, IntAttributes, FloatAttributes, sizeof(Formats) / sizeof(*Formats), Formats, &NbFormats) != 0;
            }

            if (!IsValid || (NbFormats == 0))
            {
                // Cannot find any pixel format supporting multisampling ; disabling antialiasing
                MessageError("CreateContext", "Failed to find a pixel format supporting antialiasing ; antialiasing will be disabled.");
                Params->AntialiasingLevel = 0;
            }
        }

        // Get the best format among the returned ones
        if (IsValid && (NbFormats > 0))
        {
            int  BestScore = 0xFFFF;
            UINT i		   = 0;
            for (i = 0; i < NbFormats; ++i)
            {
                // Get the current format's attributes
                PIXELFORMATDESCRIPTOR Attribs;
                Attribs.nSize    = sizeof(PIXELFORMATDESCRIPTOR);
                Attribs.nVersion = 1;
                DescribePixelFormat(WindowDeviceContext, Formats[i], sizeof(PIXELFORMATDESCRIPTOR), &Attribs);

                // Evaluate the current configuration
                Color = Attribs.cRedBits + Attribs.cGreenBits + Attribs.cBlueBits + Attribs.cAlphaBits;
                Score = EvaluateConfig(Mode, Params, Color, Attribs.cDepthBits, Attribs.cStencilBits, Params->AntialiasingLevel);

                // Keep it if it's better than the current best
                if (Score < BestScore)
                {
                    BestScore  = Score;
                    BestFormat = Formats[i];
                }
            }
        }
    }

    // Find a pixel format with no antialiasing, if not needed or not supported
    if (BestFormat == 0)
    {
        // Setup a pixel format descriptor from the rendering settings
        PIXELFORMATDESCRIPTOR PixelDescriptor;
        ZeroMemory(&PixelDescriptor, sizeof(PIXELFORMATDESCRIPTOR));
        PixelDescriptor.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
        PixelDescriptor.nVersion     = 1;
        PixelDescriptor.iLayerType   = PFD_MAIN_PLANE;
        PixelDescriptor.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        PixelDescriptor.iPixelType   = PFD_TYPE_RGBA;
        PixelDescriptor.cColorBits   = (BYTE)(Mode->BitsPerPixel);
        PixelDescriptor.cDepthBits   = (BYTE)(Params->DepthBits);
        PixelDescriptor.cStencilBits = (BYTE)(Params->StencilBits);

        // Get the pixel format that best matches our requirements
        BestFormat = ChoosePixelFormat(WindowDeviceContext, &PixelDescriptor);
        if (BestFormat == 0)
        {
            MessageError("CreateContext", "Failed to find a suitable pixel format for device context -- cannot create OpenGL context.");
            return;
        }
    }

    // Extract the depth and stencil bits from the chosen format
    ActualFormat.nSize    = sizeof(PIXELFORMATDESCRIPTOR);
    ActualFormat.nVersion = 1;
    DescribePixelFormat(WindowDeviceContext, BestFormat, sizeof(PIXELFORMATDESCRIPTOR), &ActualFormat);
    Params->DepthBits   = ActualFormat.cDepthBits;
    Params->StencilBits = ActualFormat.cStencilBits;

    // Set the chosen pixel format
    if (!SetPixelFormat(WindowDeviceContext, BestFormat, &ActualFormat))
    {
        MessageError("CreateContext", "Failed to set pixel format for device context -- cannot create OpenGL context.");
        return;
    }

    // Create the OpenGL context from the device context
    WindowGLContext = wglCreateContext(WindowDeviceContext);
    if (WindowGLContext == NULL)
    {
        MessageError("CreateContext", "Failed to create an OpenGL context for this window.");
        return;
    }

    // Share display lists with other contexts
    CurrentContext = wglGetCurrentContext();
    if (CurrentContext)
        wglShareLists(CurrentContext, WindowGLContext);

    // Activate the context
    WindowSetActive(true);

    // Enable multisampling
    if (Params->AntialiasingLevel > 0)
        glEnable(GL_MULTISAMPLE_ARB);
}


////////////////////////////////////////////////////////////
/// Free all the graphical resources attached to the window
////////////////////////////////////////////////////////////
void Cleanup()
{
    // Restore the previous video mode (in case we were running in fullscreen)
	ChangeDisplaySettings(NULL, 0);

    // Unhide the mouse cursor (in case it was hidden)
    WindowShowMouseCursor(true);

    // Destroy the OpenGL context
    if (WindowGLContext)
    {
        // Unbind the context before destroying it
        WindowSetActive(false);

        wglDeleteContext(WindowGLContext);
        WindowGLContext = NULL;
    }
    if (WindowDeviceContext)
    {
        ReleaseDC(WindowhWnd, WindowDeviceContext);
        WindowDeviceContext = NULL;
    }

    // Set window open state to false
    WindowIsOpened = false;
}


////////////////////////////////////////////////////////////
/// Process a Win32 event
////////////////////////////////////////////////////////////
void ProcessEvent(UINT Message, WPARAM WParam, LPARAM LParam)
{
    // Don't process any message until window is created
    if (WindowhWnd == NULL)
        return;

    switch (Message)
    {
        // Destroy event
        case WM_DESTROY :
        {
            // Here we must cleanup resources !
            Cleanup();
            break;
        }

        // Set cursor event
        case WM_SETCURSOR :
        {
            // The mouse has moved, if the cursor is in our window we must refresh the cursor
            if (LOWORD(LParam) == HTCLIENT)
                SetCursor(WindowCursor);

            break;
        }

        // Close event
        case WM_CLOSE :
        {
			// Set window open state to false
			WindowIsOpened = false;

			MutexLock(&WindowEventMutex);
            WindowEvent.Type = Closed;
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Resize event
        case WM_SIZE :
        {
            // Update window size
            RECT RectData;
            GetClientRect(WindowhWnd, &RectData);
            WindowWidth  = RectData.right - RectData.left;
            WindowHeight = RectData.bottom - RectData.top;

			MutexLock(&WindowEventMutex);
            WindowEvent.Type			= Resized;
            WindowEvent.Size.Width		= WindowWidth;
            WindowEvent.Size.Height 	= WindowHeight;
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Gain focus event
        case WM_SETFOCUS :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type = GainedFocus;
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Lost focus event
        case WM_KILLFOCUS :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type = LostFocus;
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Text event
        case WM_CHAR :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type = TextEntered;
            WindowEvent.Text.Unicode = (UInt32)(WParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Keydown event
        case WM_KEYDOWN :
        case WM_SYSKEYDOWN :
        {
            if (WindowKeyRepeatEnabled || ((LParam & (1 << 30)) == 0))
            {
				MutexLock(&WindowEventMutex);
                WindowEvent.Type        = KeyPressed;
                WindowEvent.Key.Alt     = (HIWORD(GetAsyncKeyState(VK_MENU))    != 0);
                WindowEvent.Key.Control = (HIWORD(GetAsyncKeyState(VK_CONTROL)) != 0);
                WindowEvent.Key.Shift   = (HIWORD(GetAsyncKeyState(VK_SHIFT))   != 0);

                if (WParam != VK_SHIFT)
                {
                    WindowEvent.Key.Code = VirtualKeyCode(WParam, LParam);
                }
                else
                {
                    // Special case for shift, its state can't be retrieved directly
                    enum KeyCode Code = GetShiftState(true);
                    if (Code != 0)
                        WindowEvent.Key.Code = Code;
                }

				// Set the key status
				WindowSetKeyPressed(WindowEvent.Key.Code, true);

				EventUpdated = true;
				MutexUnlock(&WindowEventMutex);
            }
            break;
        }

        // Keyup event
        case WM_KEYUP :
        case WM_SYSKEYUP :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type        = KeyReleased;
            WindowEvent.Key.Alt     = HIWORD(GetAsyncKeyState(VK_MENU))    != 0;
            WindowEvent.Key.Control = HIWORD(GetAsyncKeyState(VK_CONTROL)) != 0;
            WindowEvent.Key.Shift   = HIWORD(GetAsyncKeyState(VK_SHIFT))   != 0;

            if (WParam != VK_SHIFT)
            {
                WindowEvent.Key.Code = VirtualKeyCode(WParam, LParam);
            }
            else
            {
                // Special case for shift, its state can't be retrieved directly
                enum KeyCode Code = GetShiftState(false);
                if (Code != 0)
                    WindowEvent.Key.Code = Code;
            }

			// Release the key status
			WindowSetKeyPressed(WindowEvent.Key.Code, false);

			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);

            break;
        }

        // Mouse wheel event
        case WM_MOUSEWHEEL :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type = MouseWheelMoved;
            WindowEvent.MouseWheel.Delta = (Int16)(HIWORD(WParam)) / 120;
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse left button down event
        case WM_LBUTTONDOWN :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type               = MouseButtonPressed;
            WindowEvent.MouseButton.Button = LeftButton;
            WindowEvent.MouseButton.X      = LOWORD(LParam);
            WindowEvent.MouseButton.Y      = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse left button up event
        case WM_LBUTTONUP :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type               = MouseButtonReleased;
            WindowEvent.MouseButton.Button = LeftButton;
            WindowEvent.MouseButton.X      = LOWORD(LParam);
            WindowEvent.MouseButton.Y      = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse right button down event
        case WM_RBUTTONDOWN :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type               = MouseButtonPressed;
            WindowEvent.MouseButton.Button = RightButton;
            WindowEvent.MouseButton.X      = LOWORD(LParam);
            WindowEvent.MouseButton.Y      = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse right button up event
        case WM_RBUTTONUP :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type               = MouseButtonReleased;
            WindowEvent.MouseButton.Button = RightButton;
            WindowEvent.MouseButton.X      = LOWORD(LParam);
            WindowEvent.MouseButton.Y      = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse wheel button down event
        case WM_MBUTTONDOWN :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type               = MouseButtonPressed;
            WindowEvent.MouseButton.Button = Middle;
            WindowEvent.MouseButton.X      = LOWORD(LParam);
            WindowEvent.MouseButton.Y      = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse wheel button up event
        case WM_MBUTTONUP :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type               = MouseButtonReleased;
            WindowEvent.MouseButton.Button = Middle;
            WindowEvent.MouseButton.X      = LOWORD(LParam);
            WindowEvent.MouseButton.Y      = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse X button down event
        case WM_XBUTTONDOWN :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type               = MouseButtonPressed;
            WindowEvent.MouseButton.Button = HIWORD(WParam) == XBUTTON1 ? XButton1 : XButton2;
            WindowEvent.MouseButton.X      = LOWORD(LParam);
            WindowEvent.MouseButton.Y      = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse X button up event
        case WM_XBUTTONUP :
        {
			MutexLock(&WindowEventMutex);
            WindowEvent.Type               = MouseButtonReleased;
            WindowEvent.MouseButton.Button = HIWORD(WParam) == XBUTTON1 ? XButton1 : XButton2;
            WindowEvent.MouseButton.X      = LOWORD(LParam);
            WindowEvent.MouseButton.Y      = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse move event
        case WM_MOUSEMOVE :
        {
            // Check if we need to generate a MouseEntered event
            if (!WindowIsCursorIn)
            {
                TRACKMOUSEEVENT MouseEvent;
                MouseEvent.cbSize    = sizeof(TRACKMOUSEEVENT);
                MouseEvent.hwndTrack = WindowhWnd;
                MouseEvent.dwFlags   = TME_LEAVE;
                TrackMouseEvent(&MouseEvent);

                WindowIsCursorIn = true;
				
				MutexLock(&WindowEventMutex);
                WindowEvent.Type = MouseEntered;
				EventUpdated = true;
				MutexUnlock(&WindowEventMutex);
				break;
            }
			
			MutexLock(&WindowEventMutex);
            WindowEvent.Type        = MouseMoved;
            WindowEvent.MouseMove.X = LOWORD(LParam);
            WindowEvent.MouseMove.Y = HIWORD(LParam);
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse leave event
        case WM_MOUSELEAVE :
        {
            WindowIsCursorIn = false;
			
			MutexLock(&WindowEventMutex);
            WindowEvent.Type = MouseLeft;
			EventUpdated = true;
			MutexUnlock(&WindowEventMutex);
            break;
        }
    }
}

////////////////////////////////////////////////////////////
/// Check the state of the shift keys on a key event,
/// and return the corresponding KeyCode
////////////////////////////////////////////////////////////
enum KeyCode GetShiftState(bool KeyDown)
{
    static bool LShiftPrevDown = false;
    static bool RShiftPrevDown = false;

    bool LShiftDown = (HIWORD(GetAsyncKeyState(VK_LSHIFT)) != 0);
    bool RShiftDown = (HIWORD(GetAsyncKeyState(VK_RSHIFT)) != 0);

    enum KeyCode Code = 0;
    if (KeyDown)
    {
        if      (!LShiftPrevDown && LShiftDown) Code = LShift;
        else if (!RShiftPrevDown && RShiftDown) Code = RShift;
    }
    else
    {
        if      (LShiftPrevDown && !LShiftDown) Code = LShift;
        else if (RShiftPrevDown && !RShiftDown) Code = RShift;
    }

    LShiftPrevDown = LShiftDown;
    RShiftPrevDown = RShiftDown;

    return Code;
}


////////////////////////////////////////////////////////////
/// Convert a Win32 virtual key code to a KeyCode
////////////////////////////////////////////////////////////
enum KeyCode VirtualKeyCode(WPARAM VirtualKey, LPARAM Flags)
{
    switch (VirtualKey)
    {
        // VK_SHIFT is handled by the GetShiftState function
        case VK_MENU :       return (Flags & (1 << 24)) ? RAlt     : LAlt;
        case VK_CONTROL :    return (Flags & (1 << 24)) ? RControl : LControl;
        case VK_LWIN :       return LSystem;
        case VK_RWIN :       return RSystem;
        case VK_APPS :       return Menu;
        case VK_OEM_1 :      return SemiColon;
        case VK_OEM_2 :      return Slash;
        case VK_OEM_PLUS :   return Equal;
        case VK_OEM_MINUS :  return Dash;
        case VK_OEM_4 :      return LBracket;
        case VK_OEM_6 :      return RBracket;
        case VK_OEM_COMMA :  return Comma;
        case VK_OEM_PERIOD : return Period;
        case VK_OEM_7 :      return Quote;
        case VK_OEM_5 :      return BackSlash;
        case VK_OEM_3 :      return Tilde;
        case VK_ESCAPE :     return Esc;
        case VK_SPACE :      return Space;
        case VK_RETURN :     return Return;
        case VK_BACK :       return Back;
        case VK_TAB :        return Tab;
        case VK_PRIOR :      return PageUp;
        case VK_NEXT :       return PageDown;
        case VK_END :        return End;
        case VK_HOME :       return Home;
        case VK_INSERT :     return Insert;
        case VK_DELETE :     return Delete;
        case VK_ADD :        return Add;
        case VK_SUBTRACT :   return Subtract;
        case VK_MULTIPLY :   return Multiply;
        case VK_DIVIDE :     return Divide;
        case VK_PAUSE :      return Pause;
        case VK_F1 :         return F1;
        case VK_F2 :         return F2;
        case VK_F3 :         return F3;
        case VK_F4 :         return F4;
        case VK_F5 :         return F5;
        case VK_F6 :         return F6;
        case VK_F7 :         return F7;
        case VK_F8 :         return F8;
        case VK_F9 :         return F9;
        case VK_F10 :        return F10;
        case VK_F11 :        return F11;
        case VK_F12 :        return F12;
        case VK_F13 :        return F13;
        case VK_F14 :        return F14;
        case VK_F15 :        return F15;
        case VK_LEFT :       return Left;
        case VK_RIGHT :      return Right;
        case VK_UP :         return Up;
        case VK_DOWN :       return Down;
        case VK_NUMPAD0 :    return Numpad0;
        case VK_NUMPAD1 :    return Numpad1;
        case VK_NUMPAD2 :    return Numpad2;
        case VK_NUMPAD3 :    return Numpad3;
        case VK_NUMPAD4 :    return Numpad4;
        case VK_NUMPAD5 :    return Numpad5;
        case VK_NUMPAD6 :    return Numpad6;
        case VK_NUMPAD7 :    return Numpad7;
        case VK_NUMPAD8 :    return Numpad8;
        case VK_NUMPAD9 :    return Numpad9;
        case 'A' :           return A;
        case 'Z' :           return Z;
        case 'E' :           return E;
        case 'R' :           return R;
        case 'T' :           return T;
        case 'Y' :           return Y;
        case 'U' :           return U;
        case 'I' :           return I;
        case 'O' :           return O;
        case 'P' :           return P;
        case 'Q' :           return Q;
        case 'S' :           return S;
        case 'D' :           return D;
        case 'F' :           return F;
        case 'G' :           return G;
        case 'H' :           return H;
        case 'J' :           return J;
        case 'K' :           return K;
        case 'L' :           return L;
        case 'M' :           return M;
        case 'W' :           return W;
        case 'X' :           return X;
        case 'C' :           return C;
        case 'V' :           return V;
        case 'B' :           return B;
        case 'N' :           return N;
        case '0' :           return Num0;
        case '1' :           return Num1;
        case '2' :           return Num2;
        case '3' :           return Num3;
        case '4' :           return Num4;
        case '5' :           return Num5;
        case '6' :           return Num6;
        case '7' :           return Num7;
        case '8' :           return Num8;
        case '9' :           return Num9;
    }

    return 0;
}

////////////////////////////////////////////////////////////
/// Check if the current version of the OS supports unicode
/// messages and functions ; Windows 95/98/Me may not support
/// it, whereas Windows NT/2000/XP/Vista will
////////////////////////////////////////////////////////////
bool HasUnicodeSupport()
{
    OSVERSIONINFO VersionInfo;
    ZeroMemory(&VersionInfo, sizeof(VersionInfo));
    VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);

    if (GetVersionEx(&VersionInfo))
    {
        return VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////
/// Function called whenever one of our windows receives a message
////////////////////////////////////////////////////////////
static LRESULT CALLBACK GlobalOnEvent(HWND Handle, UINT Message, WPARAM WParam, LPARAM LParam)
{
    // Associate handle and Window instance when the creation message is received
    if (Message == WM_CREATE)
    {
        // Get instance (it was passed as the last argument of CreateWindow)
        LONG Instance = (LONG)((CREATESTRUCT *)LParam)->lpCreateParams;

        // Set as the "user data" parameter of the window
        SetWindowLongPtr(Handle, GWLP_USERDATA, Instance);
    }

    // Forward the event to the appropriate function
	if (WindowhWnd)
	{
		ProcessEvent(Message, WParam, LParam);
		
		if (WindowCallback)
			return CallWindowProc((WNDPROC)(WindowCallback), Handle, Message, WParam, LParam);
	}

    // We don't forward the WM_CLOSE message to prevent the OS from automatically destroying the window
    if (Message == WM_CLOSE)
        return 0;

    return HasUnicodeSupport() ? DefWindowProcW(Handle, Message, WParam, LParam) :
								 DefWindowProcA(Handle, Message, WParam, LParam);
}

////////////////////////////////////////////////////////////
/// Evaluate a pixel format configuration.
/// This functions can be used by implementations that have
/// several valid formats and want to get the best one
////////////////////////////////////////////////////////////
int EvaluateConfig(struct VideoMode * Mode, struct WindowSettings * Settings, int ColorBits, int DepthBits, int StencilBits, int Antialiasing)
{
    return abs((int)(Mode->BitsPerPixel          - ColorBits))   +
           abs((int)(Settings->DepthBits         - DepthBits))   +
           abs((int)(Settings->StencilBits       - StencilBits)) +
           abs((int)(Settings->AntialiasingLevel - Antialiasing));
}

////////////////////////////////////////////////////////////
/// Get the event struct from the window
////////////////////////////////////////////////////////////
void WindowGetEvent(struct Event * Copy)
{
	// NOTE: We only can get the event one time if it was updated
	//		 In the case that we would need to update from different threads, remove it
	if (EventUpdated)
	{
		MutexLock(&WindowEventMutex);

		memcpy(Copy, &WindowEvent, sizeof(struct Event));

		memset(&WindowEvent, 0, sizeof(struct Event));

		MutexUnlock(&WindowEventMutex);

		EventUpdated = false;
	}
}

////////////////////////////////////////////////////////////
/// Set the pressed status of a key
////////////////////////////////////////////////////////////
void WindowSetKeyPressed(enum Key Code, bool Pressed)
{
	KeyArray[Code] = Pressed;
}

////////////////////////////////////////////////////////////
/// Get the pressed status of a key
////////////////////////////////////////////////////////////
bool WindowIsKeyPressed(enum Key Code)
{
	return KeyArray[Code];
}
