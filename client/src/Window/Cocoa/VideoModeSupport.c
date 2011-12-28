////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Window/VideoModeSupport.h>
#include <ApplicationServices/ApplicationServices.h>

////////////////////////////////////////////////////////////
/// Get supported video modes
////////////////////////////////////////////////////////////
void VideoModeGetDesktopMode(struct VideoMode * Mode)
{
	// Ceylo -- using same implementation as in OSXCarbon
	
    // Enumerate all available video modes for primary display adapter
    CFArrayRef DisplayModes = CGDisplayAvailableModes( kCGDirectMainDisplay );
    CFIndex DisplayModeCount = CFArrayGetCount( DisplayModes );
    CFDictionaryRef CurrentMode;
	
	int Count = 0;

    for (Count = 0; Count < DisplayModeCount; ++Count)
    {
        CurrentMode = (CFDictionaryRef)CFArrayGetValueAtIndex( DisplayModes, Count );
		
		struct VideoMode Mode;

        CFNumberGetValue((CFNumberRef)CFDictionaryGetValue(CurrentMode, kCGDisplayWidth), kCFNumberIntType, &(Mode.Width));
        CFNumberGetValue((CFNumberRef)CFDictionaryGetValue(CurrentMode, kCGDisplayHeight), kCFNumberIntType, &(Mode.Height));
        CFNumberGetValue((CFNumberRef)CFDictionaryGetValue(CurrentMode, kCGDisplayBitsPerPixel), kCFNumberIntType, &(Mode.BitsPerPixel));

    	if (ModeList->Modes == NULL)
    	{
			ModeList->Modes = (struct VideoMode*)(malloc(sizeof(struct VideoMode)));
			ModeList->Modes[0] = Mode;
    	}

		int ModeCounter = 0;

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
	// Ceylo -- using same implementation as in OSXCarbon
	
	CFDictionaryRef CurrentVideoMode = CGDisplayCurrentMode(kCGDirectMainDisplay);
	
	// Get video mode width
    CFNumberGetValue((CFNumberRef)CFDictionaryGetValue(CurrentVideoMode, kCGDisplayWidth),
					 kCFNumberIntType,
					 Mode.Width);
	
	// Get video mode height
    CFNumberGetValue((CFNumberRef)CFDictionaryGetValue(CurrentVideoMode, kCGDisplayHeight),
					 kCFNumberIntType,
					 Mode.Height);
	
	// Get video mode depth
    CFNumberGetValue((CFNumberRef)CFDictionaryGetValue(CurrentVideoMode, kCGDisplayBitsPerPixel),
					 kCFNumberIntType,
					 Mode.BitsPerPixel);
}
