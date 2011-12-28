#ifndef WINDOW_VIDEOMODESUPPORT_H
#define WINDOW_VIDEOMODESUPPORT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

struct VideoMode
{
    UInt32 Width;        ///< Video mode width, in pixels
    UInt32 Height;       ///< Video mode height, in pixels
    UInt32 BitsPerPixel; ///< Video mode pixel depth, in bits per pixels (32 by default)
};

struct VideoModeStruct
{
	UInt32 Size;
	struct VideoMode * Modes;
};

struct VideoModeStruct VideoModeListDefault;

////////////////////////////////////////////////////////////
/// Get supported video modes
///
/// \param Modes : Array to fill with available video modes
///
////////////////////////////////////////////////////////////
#if defined(SYSTEM_WINDOWS) || defined(SYSTEM_MACOS)

    void VideoModeGetSupportedModes(struct VideoModeStruct * ModeList);

#elif defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

    #include <X11/Xlib.h>
    #include <X11/extensions/Xrandr.h>

    void VideoModeGetSupportedModes(struct VideoModeStruct * ModeList, Display* Disp, Int32 Screen, XIM * InputMethod);

#endif

////////////////////////////////////////////////////////////
/// Get current desktop video mode
///
/// \return Current desktop video mode
///
////////////////////////////////////////////////////////////
#if defined(SYSTEM_WINDOWS) || defined(SYSTEM_MACOS)

    void VideoModeGetDesktopMode(struct VideoMode * Mode);

#elif defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

    void VideoModeGetDesktopMode(struct VideoMode * Mode, Display* Disp, int * Screen, XIM * InputMethod);

#endif

////////////////////////////////////////////////////////////
/// Open the display (if not already done) (X11 Impl)
///
/// \return True if the display is properly opened
///
////////////////////////////////////////////////////////////
#if defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

	bool OpenDisplay(Display* Disp, Int32 * Screen, XIM * InputMethod);

#endif

#endif // WINDOW_VIDEOMODESUPPORT_H
