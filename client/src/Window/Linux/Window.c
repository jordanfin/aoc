////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Window/Window.h>
#include <X11/keysym.h>
#include <X11/extensions/Xrandr.h>
#include <GL/glx.h>

////////////////////////////////////////////////////////////
/// Filter the events received by windows
/// (only allow those matching a specific window)
////////////////////////////////////////////////////////////
static Bool CheckEvent(Display * Disp, XEvent * Event, XPointer UserData)
{
    // Just check if the event matches the window
    return (Event->xany.window == (Window)(UserData));
}

////////////////////////////////////////////////////////////
// Static member data
////////////////////////////////////////////////////////////
static int     WindowAttributeList[]  = { GLX_RGBA, GLX_DOUBLEBUFFER,
    GLX_RED_SIZE, 4,
    GLX_GREEN_SIZE, 4,
    GLX_BLUE_SIZE, 4,
    GLX_DEPTH_SIZE, 24, /* TODO: so, check the sizes of the depth, r, g, b, and doublebuff, rgba, (etc..) and set it during the initialization */
    None };
Window         WindowX11              = 0;      ///< X11 structure defining our window
GLXContext     WindowGLContext        = NULL;   ///< OpenGL context attached to the window
Atom           WindowAtomClose        = 0;      ///< Atom used to identify the close event
int            WindowOldVideoMode     = -1;     ///< Video mode in use before we switch to fullscreen
Cursor         WindowHiddenCursor     = 0;      ///< As X11 doesn't provide cursor hidding, we must create a transparent one
XIC            WindowInputContext     = NULL;   ///< Input context used to get unicode input in our window
bool           WindowKeyRepeat        = 0;      ///< Is the KeyRepeat feature enabled ?
XEvent         WindowLastKeyReleaseEvent;       ///< Last key release event we received (needed for discarding repeated key events)
bool           WindowFullscreen       = false;  ///< Show if the window is in fullscreen mode currently
struct Event   WindowEvent			  = {{0}};	///< Window input
Mutex          WindowEventMutex;                ///< Event input mutex
bool           EventUpdated           = false;  ///< Check if the event was updated
Display *      WindowDisp             = NULL;
int            WindowScreen           = 0;
XIM            WindowInputMethod      = NULL;
unsigned long  WindowEventMask        = FocusChangeMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
                                        PointerMotionMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask |
                                        EnterWindowMask | LeaveWindowMask;

////////////////////////////////////////////////////////////
// Internal methods
////////////////////////////////////////////////////////////
void CleanUp();			                            ///< Free all the graphical resources attached to the window
void ProcessEvent(XEvent * WinEvent);               ///< Process an incoming event from the window
void SwitchToFullscreen(struct VideoMode * Mode);   ///< Switch to fullscreen mode
void CreateHiddenCursor();                          ///< Create a transparent mouse cursor
enum KeyCode VirtualKeysym(KeySym Sym);             ///< Convert a X11 keysym to a KeyCode

////////////////////////////////////////////////////////////
/// Create (or recreate) the window
////////////////////////////////////////////////////////////
bool WindowCreate(struct VideoMode * Mode, char * Title, unsigned long WindowStyle, struct WindowSettings * Params)
{

    int Left, Top, glxMajor, glxMinor;
    bool bFullscreen = ((WindowStyle & Fullscreen) != 0);
    XVisualInfo * Visual;
    XSetWindowAttributes Attributes;

    // Initialize the event mutex
    MutexInitialize(&WindowEventMutex);

    WindowDisp = XOpenDisplay(NULL);

    if (WindowDisp)
    {
        WindowScreen = DefaultScreen(WindowDisp);

        // Get the input method (XIM) object
        WindowInputMethod = XOpenIM(WindowDisp, NULL, NULL, NULL);
    }
    else
    {
        MessageError("WindowCreate", "Failed to open a connection with the X server");
        return false;
    }

    // Compute position and size
    if (!bFullscreen)
    {
        Left = (DisplayWidth(WindowDisp, WindowScreen)  - Mode->Width)  / 2;
        Top  = (DisplayHeight(WindowDisp, WindowScreen) - Mode->Height) / 2;
    }
    else
    {
        Left = 0;
        Top  = 0;
    }

    WindowWidth = Mode->Width;
    WindowHeight = Mode->Height;

    // Switch to fullscreen if necessary
    if (bFullscreen)
        SwitchToFullscreen(Mode);

    // Create the rendering context
    Visual = glXChooseVisual(WindowDisp, DefaultScreen(WindowDisp), WindowAttributeList);

    // Get the GLX version
    glXQueryVersion(WindowDisp, &glxMajor, &glxMinor);
    printf("GLX Version:    %d.%d\n", glxMajor, glxMinor);

    // Create the OpenGL context
    WindowGLContext = glXCreateContext(WindowDisp, Visual, glXGetCurrentContext(), True);

    if (WindowGLContext == NULL)
    {
        MessageError("CreateContext", "Failed to create an OpenGL context for this window");
        return false;
    }

    // Activate the context
    WindowSetActive(true);

    // Define the window attributes
    Attributes.event_mask        = WindowEventMask;
    Attributes.colormap          = XCreateColormap(WindowDisp, RootWindow(WindowDisp, Visual->screen), Visual->visual, AllocNone);
    Attributes.border_pixel      = 0;
    Attributes.override_redirect = bFullscreen;

    // Create the window
    WindowX11 = XCreateWindow(WindowDisp,
                             RootWindow(WindowDisp, WindowScreen),
                             Left, Top,
                             WindowWidth, WindowHeight,
                             0,
                             Visual->depth,
                             InputOutput,
                             Visual->visual,
                             CWEventMask | CWColormap | CWOverrideRedirect, &Attributes);
    if (!WindowX11)
    {
        MessageError("WindowCreate", "Failed to create window");
        return false;
    }

    // Set the window's name
    XStoreName(WindowDisp, WindowX11, Title);

    // Set the window's style (tell the windows manager to change our window's decorations and functions according to the requested style)
    if (!bFullscreen)
    {
        Atom WMHintsAtom = XInternAtom(WindowDisp, "_MOTIF_WM_HINTS", False);

        if (WMHintsAtom)
        {
            static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
            static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

            static const unsigned long MWM_DECOR_BORDER      = 1 << 1;
            static const unsigned long MWM_DECOR_RESIZEH     = 1 << 2;
            static const unsigned long MWM_DECOR_TITLE       = 1 << 3;
            static const unsigned long MWM_DECOR_MENU        = 1 << 4;
            static const unsigned long MWM_DECOR_MINIMIZE    = 1 << 5;
            static const unsigned long MWM_DECOR_MAXIMIZE    = 1 << 6;

            static const unsigned long MWM_FUNC_RESIZE       = 1 << 1;
            static const unsigned long MWM_FUNC_MOVE         = 1 << 2;
            static const unsigned long MWM_FUNC_MINIMIZE     = 1 << 3;
            static const unsigned long MWM_FUNC_MAXIMIZE     = 1 << 4;
            static const unsigned long MWM_FUNC_CLOSE        = 1 << 5;

            struct WMHints
            {
                unsigned long Flags;
                unsigned long Functions;
                unsigned long Decorations;
                long          InputMode;
                unsigned long State;
            };

            struct WMHints Hints;
            Hints.Flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
            Hints.Decorations = 0;
            Hints.Functions   = 0;

            if (WindowStyle & Titlebar)
            {
                Hints.Decorations |= MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MINIMIZE | MWM_DECOR_MENU;
                Hints.Functions   |= MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE;
            }
            if (WindowStyle & Resize)
            {
                Hints.Decorations |= MWM_DECOR_MAXIMIZE | MWM_DECOR_RESIZEH;
                Hints.Functions   |= MWM_FUNC_MAXIMIZE | MWM_FUNC_RESIZE;
            }
            if (WindowStyle & Close)
            {
                Hints.Decorations |= 0;
                Hints.Functions   |= MWM_FUNC_CLOSE;
            }

            XChangeProperty(WindowDisp, WindowX11, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, (const unsigned char*)(&Hints), 5);
        }

        // This is a hack to force some windows managers to disable resizing
        if (!(WindowStyle & Resize))
        {
            XSizeHints XSizeHints;
            XSizeHints.flags      = PMinSize | PMaxSize;
            XSizeHints.min_width  = XSizeHints.max_width  = WindowWidth;
            XSizeHints.min_height = XSizeHints.max_height = WindowHeight;
            XSetWMNormalHints(WindowDisp, WindowX11, &XSizeHints);
        }
    }

    // Make sure the "last key release" is initialized with invalid values
    WindowLastKeyReleaseEvent.type = -1;

    // Get the atom defining the close event
    WindowAtomClose = XInternAtom(WindowDisp, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(WindowDisp, WindowX11, &WindowAtomClose, 1);

    // Create the input context
    if (WindowInputMethod)
    {
        WindowInputContext = XCreateIC(WindowInputMethod,
                                       XNClientWindow,  WindowX11,
                                       XNFocusWindow,   WindowX11,
                                       XNInputStyle,    XIMPreeditNothing  | XIMStatusNothing,
                                       NULL);

        if (!WindowInputContext)
            MessageError("WindowCreate", "Failed to create input context for window. TextEntered event won't be able to return unicode");
    }

    // Show the window
    XMapWindow(WindowDisp, WindowX11);
    XFlush(WindowDisp);

    // Create the hiden cursor
    CreateHiddenCursor();

    WindowSetActive(true);

    // Check if it has direct rendering support
    if (glXIsDirect(WindowDisp, WindowGLContext) == True)
        printf("DRI:            Direct rendering is enabled\n");
    else
        printf("DRI:            No Direct rendering available\n");

    // Flush the commands queue
     XFlush(WindowDisp);

    // In fullscreen mode, we must grab keyboard and mouse inputs
    if (bFullscreen)
    {
        XGrabPointer(WindowDisp, WindowX11, true, 0, GrabModeAsync, GrabModeAsync, WindowX11, None, CurrentTime);
        XGrabKeyboard(WindowDisp, WindowX11, true, GrabModeAsync, GrabModeAsync, CurrentTime);
    }

    return true;
}


////////////////////////////////////////////////////////////
/// Close (destroy) the window
////////////////////////////////////////////////////////////
void WindowClose()
{
    // Destroy the event mutex
    MutexDestroy(&WindowEventMutex);

    // Cleanup graphical resources
    CleanUp();

    // Destroy the input context
    if (WindowInputContext)
    {
        XDestroyIC(WindowInputContext);
    }

    // Destroy the window
    if (WindowX11)
    {
        XDestroyWindow(WindowDisp, WindowX11);
        XFlush(WindowDisp);
    }

    // Close the input method object
    if (WindowInputMethod)
        XCloseIM(WindowInputMethod);

    XCloseDisplay(WindowDisp);
    WindowDisp = NULL;
}


////////////////////////////////////////////////////////////
/// Check if there's an active context on the current thread
////////////////////////////////////////////////////////////
bool WindowIsContextActive()
{
    return glXGetCurrentContext() != NULL;
}

////////////////////////////////////////////////////////////
/// Process incoming events from operating system
////////////////////////////////////////////////////////////
void WindowProcessEvents()
{
    // This function implements a workaround to properly discard
    // repeated key events when necessary. The problem is that the
    // system's key events policy doesn't match the engine's one: X server will generate
    // both repeated KeyPress and KeyRelease events when maintaining a key down, while
    // the engine only wants repeated KeyPress events. Thus, we have to:
    // - Discard duplicated KeyRelease events when EnableKeyRepeat is true
    // - Discard both duplicated KeyPress and KeyRelease events when EnableKeyRepeat is false
    XEvent Event;

    // If we don't do the XSync, the drawing and the
	// registration of keyboard events will typically start lagging behind seriously
    XSync(WindowDisp, False);

    // Process any event in the queue matching our window
    while (XCheckIfEvent(WindowDisp, &Event, &CheckEvent, (XPointer)WindowX11))
    {
        // Detect repeated key events
        if ((Event.type == KeyPress) || (Event.type == KeyRelease))
        {
            if (Event.xkey.keycode < 256)
            {
                // To detect if it is a repeated key event, we check the current state of the key.
                // - If the state is "down", KeyReleased events must obviously be discarded.
                // - KeyPress events are a little bit harder to handle: they depend on the EnableKeyRepeat state,
                //   and we need to properly forward the first one.
                char Keys[32];
                XQueryKeymap(WindowDisp, Keys);
                if (Keys[Event.xkey.keycode >> 3] & (1 << (Event.xkey.keycode % 8)))
                {
                    // KeyRelease event + key down = repeated event --> discard
                    if (Event.type == KeyRelease)
                    {
                        memcpy(&WindowLastKeyReleaseEvent, &Event, sizeof(XEvent));
                        continue;
                    }

                    // KeyPress event + key repeat disabled + matching KeyRelease event = repeated event --> discard
                    if ((Event.type == KeyPress) && !WindowKeyRepeat &&
                        (WindowLastKeyReleaseEvent.xkey.keycode == Event.xkey.keycode) &&
                        (WindowLastKeyReleaseEvent.xkey.time == Event.xkey.time))
                    {
                        continue;
                    }
                }
            }
        }

        // Process the event
        ProcessEvent(&Event);
   }
}

////////////////////////////////////////////////////////////
/// Process an incoming event from the window
////////////////////////////////////////////////////////////
void ProcessEvent(XEvent * WinEvent)
{
    switch (WinEvent->type)
    {
        case Expose :
        {
            if (WinEvent->xexpose.count != 0)
                break;

            // Update render ?
            break;
        }

        // Destroy event
        case DestroyNotify :
        {
            // The window is about to be destroyed : we must cleanup resources
            CleanUp();
            break;
        }

        // Gain focus event
        case FocusIn :
        {
            // Update the input context
            if (WindowInputContext)
                XSetICFocus(WindowInputContext);

            MutexLock(&WindowEventMutex);
            WindowEvent.Type = GainedFocus;
            EventUpdated = true;
            MutexUnlock(&WindowEventMutex);
            break;
        }

        // Lost focus event
        case FocusOut :
        {
            // Update the input context
            if (WindowInputContext)
                XUnsetICFocus(WindowInputContext);

            MutexLock(&WindowEventMutex);
            WindowEvent.Type = LostFocus;
            EventUpdated = true;
            MutexUnlock(&WindowEventMutex);
            break;
        }

        // Resize event
        case ConfigureNotify :
        {
            if ((WinEvent->xconfigure.width != (int)WindowWidth) || (WinEvent->xconfigure.height != (int)WindowHeight))
            {
                WindowWidth  = WinEvent->xconfigure.width;
                WindowHeight = WinEvent->xconfigure.height;

                MutexLock(&WindowEventMutex);
                WindowEvent.Type        = Resized;
                WindowEvent.Size.Width  = WindowWidth;
                WindowEvent.Size.Height = WindowHeight;
                EventUpdated = true;
                MutexUnlock(&WindowEventMutex);
            }
            break;
        }

        // Close event
        case ClientMessage :
        {
            if ((WinEvent->xclient.format == 32) && (WinEvent->xclient.data.l[0]) == (long)WindowAtomClose)
            {
                MutexLock(&WindowEventMutex);
                WindowEvent.Type = Closed;
                EventUpdated = true;
                MutexUnlock(&WindowEventMutex);
            }
            break;
        }

        // Key down event
        case KeyPress :
        {
            // Get the keysym of the key that has been pressed
            static XComposeStatus KeyboardStatus;
            char Buffer[32];
            KeySym Sym;
            XLookupString(&WinEvent->xkey, Buffer, sizeof(Buffer), &Sym, &KeyboardStatus);

            // Fill the event parameters
            MutexLock(&WindowEventMutex);
            WindowEvent.Type        = KeyPressed;
            WindowEvent.Key.Code    = VirtualKeysym(Sym);
            WindowEvent.Key.Alt     = WinEvent->xkey.state & Mod1Mask;
            WindowEvent.Key.Control = WinEvent->xkey.state & ControlMask;
            WindowEvent.Key.Shift   = WinEvent->xkey.state & ShiftMask;

            // Generate a TextEntered event
            if (!XFilterEvent(WinEvent, None))
            {
                static XComposeStatus ComposeStatus;
                char KeyBuffer[16];
                if (XLookupString(&WinEvent->xkey, KeyBuffer, sizeof(KeyBuffer), NULL, &ComposeStatus))
                {
                    WindowEvent.Type         = TextEntered;
                    WindowEvent.Text.Unicode = (UInt32)KeyBuffer[0];
                }
            }

            EventUpdated = true;
            MutexUnlock(&WindowEventMutex);
            break;
        }

        // Key up event
        case KeyRelease :
        {
            // Get the keysym of the key that has been pressed
            char Buffer[32];
            KeySym Sym;
            XLookupString(&WinEvent->xkey, Buffer, 32, &Sym, NULL);

            // Fill the event parameters
            MutexLock(&WindowEventMutex);
            WindowEvent.Type        = KeyReleased;
            WindowEvent.Key.Code    = VirtualKeysym(Sym);
            WindowEvent.Key.Alt     = WinEvent->xkey.state & Mod1Mask;
            WindowEvent.Key.Control = WinEvent->xkey.state & ControlMask;
            WindowEvent.Key.Shift   = WinEvent->xkey.state & ShiftMask;
            EventUpdated = true;
            MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse button pressed
        case ButtonPress :
        {
            unsigned int Button = WinEvent->xbutton.button;
            if ((Button == Button1) || (Button == Button2) || (Button == Button3) || (Button == 8) || (Button == 9))
            {
                MutexLock(&WindowEventMutex);
                WindowEvent.Type          = MouseButtonPressed;
                WindowEvent.MouseButton.X = WinEvent->xbutton.x;
                WindowEvent.MouseButton.Y = WinEvent->xbutton.y;

                switch (Button)
                {
                    case Button1 : WindowEvent.MouseButton.Button = Left;     break;
                    case Button2 : WindowEvent.MouseButton.Button = Middle;   break;
                    case Button3 : WindowEvent.MouseButton.Button = Right;    break;
                    case 8 :       WindowEvent.MouseButton.Button = XButton1; break;
                    case 9 :       WindowEvent.MouseButton.Button = XButton2; break;
                }

                EventUpdated = true;
                MutexUnlock(&WindowEventMutex);
            }
            break;
        }

        // Mouse button released
        case ButtonRelease :
        {
            unsigned int Button = WinEvent->xbutton.button;

            MutexLock(&WindowEventMutex);

            if ((Button == Button1) || (Button == Button2) || (Button == Button3) || (Button == 8) || (Button == 9))
            {
                WindowEvent.Type          = MouseButtonReleased;
                WindowEvent.MouseButton.X = WinEvent->xbutton.x;
                WindowEvent.MouseButton.Y = WinEvent->xbutton.y;
                switch (Button)
                {
                    case Button1 : WindowEvent.MouseButton.Button = Left;     break;
                    case Button2 : WindowEvent.MouseButton.Button = Middle;   break;
                    case Button3 : WindowEvent.MouseButton.Button = Right;    break;
                    case 8 :       WindowEvent.MouseButton.Button = XButton1; break;
                    case 9 :       WindowEvent.MouseButton.Button = XButton2; break;
                }
            }
            else if ((Button == Button4) || (Button == Button5))
            {
                WindowEvent.Type             = MouseWheelMoved;
                WindowEvent.MouseWheel.Delta = WinEvent->xbutton.button == Button4 ? 1 : -1;
            }

            EventUpdated = true;
            MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse moved
        case MotionNotify :
        {
            MutexLock(&WindowEventMutex);
            WindowEvent.Type        = MouseMoved;
            WindowEvent.MouseMove.X = WinEvent->xmotion.x;
            WindowEvent.MouseMove.Y = WinEvent->xmotion.y;
            EventUpdated = true;
            MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse entered
        case EnterNotify :
        {
            MutexLock(&WindowEventMutex);
            WindowEvent.Type = MouseEntered;
            EventUpdated = true;
            MutexUnlock(&WindowEventMutex);
            break;
        }

        // Mouse left
        case LeaveNotify :
        {
            MutexLock(&WindowEventMutex);
            WindowEvent.Type = MouseLeft;
            EventUpdated = true;
            MutexUnlock(&WindowEventMutex);
            break;
        }
    }
}

////////////////////////////////////////////////////////////
/// Display the window on screen
////////////////////////////////////////////////////////////
void WindowDisplay()
{
    if (WindowX11 && WindowGLContext)
        glXSwapBuffers(WindowDisp, WindowX11);
}

////////////////////////////////////////////////////////////
/// Activate of deactivate the window as the current target
/// for rendering
////////////////////////////////////////////////////////////
void WindowSetActive(bool Active)
{
    if (Active)
    {
        if (WindowX11 && WindowGLContext && (glXGetCurrentContext() != WindowGLContext))
            glXMakeCurrent(WindowDisp, WindowX11, WindowGLContext);
    }
    else
    {
        if (glXGetCurrentContext() == WindowGLContext)
            glXMakeCurrent(WindowDisp, None, NULL);
    }
}

////////////////////////////////////////////////////////////
/// Enable / disable vertical synchronization
////////////////////////////////////////////////////////////
void WindowUseVerticalSync(bool Enabled)
{
    const GLubyte * ProcAddress = (GLubyte*)("glXSwapIntervalSGI");
    PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)(glXGetProcAddress(ProcAddress));
    if (glXSwapIntervalSGI)
        glXSwapIntervalSGI(Enabled ? 1 : 0);
}

////////////////////////////////////////////////////////////
/// Show or hide the mouse cursor
////////////////////////////////////////////////////////////
void WindowShowMouseCursor(bool Show)
{
    XDefineCursor(WindowDisp, WindowX11, Show ? None : WindowHiddenCursor);
    XFlush(WindowDisp);
}

////////////////////////////////////////////////////////////
/// Change the position of the mouse cursor
////////////////////////////////////////////////////////////
void WindowSetCursorPosition(UInt32 Left, UInt32 Top)
{
    XWarpPointer(WindowDisp, None, WindowX11, 0, 0, 0, 0, Left, Top);
    XFlush(WindowDisp);
}

////////////////////////////////////////////////////////////
/// Change the position of the window on screen
////////////////////////////////////////////////////////////
void WindowSetPosition(Int32 Left, Int32 Top)
{
    XMoveWindow(WindowDisp, WindowX11, Left, Top);
    XFlush(WindowDisp);
}

////////////////////////////////////////////////////////////
/// Change the size of the window
////////////////////////////////////////////////////////////
void WindowSetSize(UInt32 Width, UInt32 Height)
{
    XResizeWindow(WindowDisp, WindowX11, Width, Height);
    XFlush(WindowDisp);
}

////////////////////////////////////////////////////////////
/// Show or hide the window
////////////////////////////////////////////////////////////
void WindowShow(bool State)
{
    if (State)
        XMapWindow(WindowDisp, WindowX11);
    else
        XUnmapWindow(WindowDisp, WindowX11);

    XFlush(WindowDisp);
}

////////////////////////////////////////////////////////////
/// Change the window's icon
////////////////////////////////////////////////////////////
void WindowSetIcon(UInt32 Width, UInt32 Height, UInt8 * Pixels)
{
    Int32 i, j, k;
    UInt8 * IconPixels = (UInt8*)malloc(Width * Height * 4);
    Visual * DefVisual;
    unsigned int DefDepth;
    XImage * IconImage;
    Pixmap IconPixmap;
    XGCValues Values;
    GC IconGC;
    Int32 Pitch;
    static UInt8 * MaskPixels;
    XWMHints * Hints;
    Pixmap MaskPixmap;

    // X11 wants BGRA pixels : swap red and blue channels
    // Note : this memory will never be freed, but it seems to cause a bug on exit if I do so
    for (i = 0; i < Width * Height; ++i)
    {
        IconPixels[i * 4 + 0] = Pixels[i * 4 + 2];
        IconPixels[i * 4 + 1] = Pixels[i * 4 + 1];
        IconPixels[i * 4 + 2] = Pixels[i * 4 + 0];
        IconPixels[i * 4 + 3] = Pixels[i * 4 + 3];
    }

    // Create the icon pixmap
    DefVisual = DefaultVisual(WindowDisp, WindowScreen);
    DefDepth  = DefaultDepth(WindowDisp, WindowScreen);
    IconImage = XCreateImage(WindowDisp, DefVisual, DefDepth, ZPixmap, 0, (char*)IconPixels, Width, Height, 32, 0);
    if (!IconImage)
    {
        Delete(IconPixels);
        MessageError("WindowSetIcon", "Failed to set the window's icon");
        return;
    }

    IconPixmap = XCreatePixmap(WindowDisp, RootWindow(WindowDisp, WindowScreen), Width, Height, DefDepth);
    IconGC = XCreateGC(WindowDisp, IconPixmap, 0, &Values);
    XPutImage(WindowDisp, IconPixmap, IconGC, IconImage, 0, 0, 0, 0, Width, Height);
    XFreeGC(WindowDisp, IconGC);
    XDestroyImage(IconImage);

    // Create the mask pixmap (must have 1 bit depth)
    Pitch = (Width + 7) / 8;
    MaskPixels = (UInt8*)malloc(Pitch * Height);

    for (j = 0; j < Height; ++j)
    {
        for (i = 0; i < Pitch; ++i)
        {
            for (k = 0; k < 8; ++k)
            {
                if (i * 8 + k < Width)
                {
                    UInt8 Opacity = (Pixels[(i * 8 + k + j * Width) * 4 + 3] > 0) ? 1 : 0;
                    MaskPixels[i + j * Pitch] |= (Opacity << k);
                }
            }
        }
    }

    MaskPixmap = XCreatePixmapFromBitmapData(WindowDisp, WindowX11, (char*)&MaskPixels[0], Width, Height, 1, 0, 1);

    // Send our new icon to the window through the WMHints
    Hints = XAllocWMHints();
    Hints->flags       = IconPixmapHint | IconMaskHint;
    Hints->icon_pixmap = IconPixmap;
    Hints->icon_mask   = MaskPixmap;
    XSetWMHints(WindowDisp, WindowX11, Hints);
    XFree(Hints);

    XFlush(WindowDisp);

    Delete(IconPixels);
    Delete(MaskPixels);
}

////////////////////////////////////////////////////////////
/// Switch to fullscreen mode
////////////////////////////////////////////////////////////
void SwitchToFullscreen(struct VideoMode * Mode)
{
    // Check if the XRandR extension is present
    int Version;

    if (XQueryExtension(WindowDisp, "RANDR", &Version, &Version, &Version))
    {
        // Get the current configuration
        XRRScreenConfiguration * Config = XRRGetScreenInfo(WindowDisp, RootWindow(WindowDisp, WindowScreen));

        if (Config)
        {
            // Get the current rotation
            Rotation CurrentRotation;
            WindowOldVideoMode = XRRConfigCurrentConfiguration(Config, &CurrentRotation);

            // Get the available screen sizes
            int NbSizes;
            XRRScreenSize* Sizes = XRRConfigSizes(Config, &NbSizes);

            if (Sizes && (NbSizes > 0))
            {
                // Search a matching size
                Int32 i;

                for (i = 0; i < NbSizes; ++i)
                {
                    if ((Sizes[i].width == (int)Mode->Width) && (Sizes[i].height == (int)Mode->Height))
                    {
                        // Switch to fullscreen mode
                        XRRSetScreenConfig(WindowDisp, Config, RootWindow(WindowDisp, WindowScreen), i, CurrentRotation, CurrentTime);

                        // Set the fullscreen value
                        WindowFullscreen = true;
                        break;
                    }
                }
            }

            // Free the configuration instance
            XRRFreeScreenConfigInfo(Config);
        }
        else
        {
            // Failed to get the screen configuration
            MessageError("SwitchToFullscreen", "Failed to get the current screen configuration for fullscreen mode, switching to windiw mode");
        }
    }
    else
    {
        // XRandr extension is not supported : we cannot use fullscreen mode
        MessageError("SwitchToFullscreen", "Fullscreen is not supported, switching to window mode");
    }
}

////////////////////////////////////////////////////////////
/// Create a transparent mouse cursor
////////////////////////////////////////////////////////////
void CreateHiddenCursor()
{
    // Create the cursor's pixmap (1x1 pixels)
	Pixmap CursorPixmap = XCreatePixmap(WindowDisp, WindowX11, 1, 1, 1);
    GC GraphicsContext = XCreateGC(WindowDisp, CursorPixmap, 0, NULL);
    XColor Color = { 0, 0, 0, 0, DoRed | DoGreen | DoBlue, 0 };
    XDrawPoint(WindowDisp, CursorPixmap, GraphicsContext, 0, 0);
    XFreeGC(WindowDisp, GraphicsContext);

    // Create the cursor, using the pixmap as both the shape and the mask of the cursor
    WindowHiddenCursor = XCreatePixmapCursor(WindowDisp, CursorPixmap, CursorPixmap, &Color, &Color, 0, 0);

    // We don't need the pixmap any longer, free it
	XFreePixmap(WindowDisp, CursorPixmap);
}


////////////////////////////////////////////////////////////
/// Cleanup graphical resources attached to the window
////////////////////////////////////////////////////////////
void CleanUp()
{
    // Restore the previous video mode (in case we were running in fullscreen)
    if (WindowFullscreen)
    {
        // Get current screen info
        XRRScreenConfiguration * Config = XRRGetScreenInfo(WindowDisp, RootWindow(WindowDisp, WindowScreen));

        if (Config)
        {
            // Get the current rotation
            Rotation CurrentRotation;
            XRRConfigCurrentConfiguration(Config, &CurrentRotation);

            // Reset the video mode
            XRRSetScreenConfig(WindowDisp, Config, RootWindow(WindowDisp, WindowScreen), WindowOldVideoMode, CurrentRotation, CurrentTime);

            // Free the configuration instance
            XRRFreeScreenConfigInfo(Config);
        }

        // Reset the fullscreen window
        WindowFullscreen = false;
    }

    // Unhide the mouse cursor (in case it was hidden)
    WindowShowMouseCursor(true);

    // Destroy the OpenGL context
    if (WindowGLContext)
    {
        glXDestroyContext(WindowDisp, WindowGLContext);
        WindowGLContext = NULL;
    }
}

////////////////////////////////////////////////////////////
/// Convert a X11 keysym to a KeyCode
////////////////////////////////////////////////////////////
enum KeyCode VirtualKeysym(KeySym Sym)
{
    // First convert to uppercase (to avoid dealing with two different keysyms for the same key)
    KeySym Lower, Key;
    XConvertCase(Sym, &Lower, &Key);

    switch (Key)
    {
        case XK_Shift_L :      return LShift;
        case XK_Shift_R :      return RShift;
        case XK_Control_L :    return LControl;
        case XK_Control_R :    return RControl;
        case XK_Alt_L :        return LAlt;
        case XK_Alt_R :        return RAlt;
        case XK_Super_L :      return LSystem;
        case XK_Super_R :      return RSystem;
        case XK_Menu :         return Menu;
        case XK_Escape :       return Esc;
        case XK_semicolon :    return SemiColon;
        case XK_slash :        return Slash;
        case XK_equal :        return Equal;
        case XK_minus :        return Dash;
        case XK_bracketleft :  return LBracket;
        case XK_bracketright : return RBracket;
        case XK_comma :        return Comma;
        case XK_period :       return Period;
        case XK_dead_acute :   return Quote;
        case XK_backslash :    return BackSlash;
        case XK_dead_grave :   return Tilde;
        case XK_space :        return Space;
        case XK_Return :       return Return;
        case XK_KP_Enter :     return Return;
        case XK_BackSpace :    return Back;
        case XK_Tab :          return Tab;
        case XK_Prior :        return PageUp;
        case XK_Next :         return PageDown;
        case XK_End :          return End;
        case XK_Home :         return Home;
        case XK_Insert :       return Insert;
        case XK_Delete :       return Delete;
        case XK_KP_Add :       return Add;
        case XK_KP_Subtract :  return Subtract;
        case XK_KP_Multiply :  return Multiply;
        case XK_KP_Divide :    return Divide;
        case XK_Pause :        return Pause;
        case XK_F1 :           return F1;
        case XK_F2 :           return F2;
        case XK_F3 :           return F3;
        case XK_F4 :           return F4;
        case XK_F5 :           return F5;
        case XK_F6 :           return F6;
        case XK_F7 :           return F7;
        case XK_F8 :           return F8;
        case XK_F9 :           return F9;
        case XK_F10 :          return F10;
        case XK_F11 :          return F11;
        case XK_F12 :          return F12;
        case XK_F13 :          return F13;
        case XK_F14 :          return F14;
        case XK_F15 :          return F15;
        case XK_Left :         return Left;
        case XK_Right :        return Right;
        case XK_Up :           return Up;
        case XK_Down :         return Down;
        case XK_KP_0 :         return Numpad0;
        case XK_KP_1 :         return Numpad1;
        case XK_KP_2 :         return Numpad2;
        case XK_KP_3 :         return Numpad3;
        case XK_KP_4 :         return Numpad4;
        case XK_KP_5 :         return Numpad5;
        case XK_KP_6 :         return Numpad6;
        case XK_KP_7 :         return Numpad7;
        case XK_KP_8 :         return Numpad8;
        case XK_KP_9 :         return Numpad9;
        case XK_A :            return A;
        case XK_Z :            return Z;
        case XK_E :            return E;
        case XK_R :            return R;
        case XK_T :            return T;
        case XK_Y :            return Y;
        case XK_U :            return U;
        case XK_I :            return I;
        case XK_O :            return O;
        case XK_P :            return P;
        case XK_Q :            return Q;
        case XK_S :            return S;
        case XK_D :            return D;
        case XK_F :            return F;
        case XK_G :            return G;
        case XK_H :            return H;
        case XK_J :            return J;
        case XK_K :            return K;
        case XK_L :            return L;
        case XK_M :            return M;
        case XK_W :            return W;
        case XK_X :            return X;
        case XK_C :            return C;
        case XK_V :            return V;
        case XK_B :            return B;
        case XK_N :            return N;
        case XK_0 :            return Num0;
        case XK_1 :            return Num1;
        case XK_2 :            return Num2;
        case XK_3 :            return Num3;
        case XK_4 :            return Num4;
        case XK_5 :            return Num5;
        case XK_6 :            return Num6;
        case XK_7 :            return Num7;
        case XK_8 :            return Num8;
        case XK_9 :            return Num9;
    }

    return 0;
}

////////////////////////////////////////////////////////////
/// Get the event struct from the window
////////////////////////////////////////////////////////////
void WindowGetEvent(struct Event * Copy)
{
    // NOTE: (see on Window/Win32/Window.c line: 1105)
    if (EventUpdated)
    {
        MutexLock(&WindowEventMutex);

        memcpy(Copy, &WindowEvent, sizeof(struct Event));

        // memset(&WindowEvent, 0, sizeof(struct Event));

        MutexUnlock(&WindowEventMutex);

        EventUpdated = false;
    }
}
