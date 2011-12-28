////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Window/VideoModeSupport.h>

////////////////////////////////////////////////////////////
/// Get supported video modes
////////////////////////////////////////////////////////////
void VideoModeGetSupportedModes(struct VideoModeStruct * ModeList, Display* Disp, int Screen, XIM * InputMethod)
{
    int Version;

    // Get the display and screen from WindowImplUnix
    OpenDisplay(Disp, &Screen, InputMethod);

    // Check if the XRandR extension is present
    if (XQueryExtension(Disp, "RANDR", &Version, &Version, &Version))
    {
        // Get the current configuration
        XRRScreenConfiguration* Config = XRRGetScreenInfo(Disp, RootWindow(Disp, Screen));

        if (Config)
        {
            // Get the available screen sizes
            Int32 NbSizes;
            XRRScreenSize * Sizes = XRRConfigSizes(Config, &NbSizes);

            if (Sizes && (NbSizes > 0))
            {
                // Get the list of supported depths
                Int32 NbDepths = 0;
                Int32 * Depths = XListDepths(Disp, Screen, &NbDepths);

                if (Depths && (NbDepths > 0))
                {
                    // Combine depths and sizes to fill the array of supported modes
					Int32 i = 0;

                    for (i = 0; i < NbDepths; ++i)
                    {
						Int32 j = 0;

                        for (j = 0; j < NbSizes; ++j)
                        {
    						struct VideoMode Mode;
    						Int32 ModeCounter = 0;

    						Mode.Width = Sizes[j].width;
    						Mode.Height = Sizes[j].height;
    						Mode.BitsPerPixel = Depths[i];

    						if (ModeList->Modes == NULL)
    						{
								ModeList->Modes = (struct VideoMode*)(malloc(sizeof(struct VideoMode)));
								ModeList->Modes[0] = Mode;
    						}

							// Add it only if it is not already in the array
    						for (ModeCounter = 0; ModeCounter <= ModeList->Size; ++ModeCounter)
    						{
    							if ( (ModeList->Modes[ModeCounter].Width != Mode.Width) &&
									 (ModeList->Modes[ModeCounter].Height != Mode.Height) &&
									 (ModeList->Modes[ModeCounter].BitsPerPixel != Mode.BitsPerPixel) )
    							{
    							    // Push back
									if (ModeCounter == ModeList->Size)
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
                }
            }

            // Free the configuration instance
            XRRFreeScreenConfigInfo(Config);
        }
        else
        {
            // Failed to get the screen configuration
			MessageError("VideoModeGetSupportedModes", "Failed to get the list of available video modes.");
        }
    }
    else
    {
        // XRandr extension is not supported : we cannot get the video modes
		MessageError("VideoModeGetSupportedModes", "Failed to get the list of available video modes.");
    }
}


////////////////////////////////////////////////////////////
/// Get current desktop video mode
////////////////////////////////////////////////////////////
void VideoModeGetDesktopMode(struct VideoMode * Mode, Display* Disp, int * Screen, XIM * InputMethod)
{
    Int32 Version;

	// Get the display and screen from WindowImplUnix
    OpenDisplay(Disp, Screen, InputMethod);

    // Check if the XRandR extension is present
    if (XQueryExtension(Disp, "RANDR", &Version, &Version, &Version))
    {
        // Get the current configuration
        XRRScreenConfiguration * Config = XRRGetScreenInfo(Disp, RootWindow(Disp, *Screen));
        if (Config)
        {
            // Get the current video mode
            Rotation CurrentRotation;
            Int32 CurrentMode = XRRConfigCurrentConfiguration(Config, &CurrentRotation);

            // Get the available screen sizes
            Int32 NbSizes;
            XRRScreenSize * Sizes = XRRConfigSizes(Config, &NbSizes);

            if (Sizes && (NbSizes > 0))
			{
				Mode->Width = Sizes[CurrentMode].width;
				Mode->Height = Sizes[CurrentMode].height;
				Mode->BitsPerPixel = DefaultDepth(Disp, *Screen);
			}

            // Free the configuration instance
            XRRFreeScreenConfigInfo(Config);
        }
    }
}

bool OpenDisplay(Display* Disp, Int32 * Screen, XIM * InputMethod)
{
    // If no display has been opened yet, open it
    if (Disp == NULL)
    {
        Disp = XOpenDisplay(NULL);

        if (Disp)
        {
            *Screen = DefaultScreen(Disp);

            // Get the input method (XIM) object
            *InputMethod = XOpenIM(Disp, NULL, NULL, NULL);
        }
        else
        {
            MessageError("OpenDisplay", "Failed to open a connection with the X server.");
        }
    }

    return (Disp != NULL);
}
