////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Window/VideoModeSupport.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
/// Get supported video modes
////////////////////////////////////////////////////////////
void VideoModeGetSupportedModes(struct VideoModeStruct* ModeList)
{
    // Enumerate all available video modes for primary display adapter
    DEVMODE Win32Mode;
	int Count, ModeCounter = 0;

    Win32Mode.dmSize = sizeof(DEVMODE);

    for (Count = 0; EnumDisplaySettings(NULL, Count, &Win32Mode); ++Count)
    {
    	struct VideoMode Mode;

    	Mode.Width = Win32Mode.dmPelsWidth;
    	Mode.Height = Win32Mode.dmPelsHeight;
    	Mode.BitsPerPixel = Win32Mode.dmBitsPerPel;

    	if (ModeList->Modes == NULL)
    	{
			ModeList->Modes = (struct VideoMode*)(malloc(sizeof(struct VideoMode)));
			ModeList->Modes[0] = Mode;
    	}

		// Add it only if it is not already in the array
    	for (ModeCounter = 0; ModeCounter <= Count; ++ModeCounter)
    	{
    		if ( (ModeList->Modes[ModeCounter].Width != Mode.Width) &&
				 (ModeList->Modes[ModeCounter].Height != Mode.Height) &&
				 (ModeList->Modes[ModeCounter].BitsPerPixel != Mode.BitsPerPixel) )
    		{
				if (ModeCounter == Count)
				{
					ModeList->Size++;

					ModeList->Modes = (struct VideoMode *)(realloc(ModeList->Modes, (ModeList->Size+1) * sizeof(struct VideoMode)));

					ModeList->Modes[ModeList->Size].Width = Mode.Width;
					ModeList->Modes[ModeList->Size].Height = Mode.Height;
					ModeList->Modes[ModeList->Size].BitsPerPixel = Mode.BitsPerPixel;

				}
    		}
    	}
	}
}


////////////////////////////////////////////////////////////
/// Get current desktop video mode
////////////////////////////////////////////////////////////
void VideoModeGetDesktopMode(struct VideoMode * Mode)
{
    DEVMODE Win32Mode;
    Win32Mode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &Win32Mode);

	Mode->Width = Win32Mode.dmPelsWidth;
	Mode->Height = Win32Mode.dmPelsHeight;
	Mode->BitsPerPixel = Win32Mode.dmBitsPerPel;
}
