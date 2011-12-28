////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Audio/OpenALHelper.h>
#include <System/IOHelper.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define ALC_ALL_DEVICES_SPECIFIER 0x1013 // Internal use

////////////////////////////////////////////////////////////
// Static member data
////////////////////////////////////////////////////////////
static ALCdevice*  AudioDevice	= NULL;
static ALCcontext* AudioContext = NULL;

////////////////////////////////////////////////////////////
/// Check the last OpenAL error
////////////////////////////////////////////////////////////
void ALCheckError(char * File, UInt32 Line)
{
    // Get the last error
    ALenum ErrorCode = alGetError();

    if (ErrorCode != AL_NO_ERROR)
    {
        char * Error = NULL;
        char * Description = NULL;

        // Decode the error code
        switch (ErrorCode)
        {
            case AL_INVALID_NAME :
            {
                Error = "AL_INVALID_NAME";
                Description = "an unacceptable name has been specified";
                break;
            }

            case AL_INVALID_ENUM :
            {
                Error = "AL_INVALID_ENUM";
                Description = "an unacceptable value has been specified for an enumerated argument";
                break;
            }

            case AL_INVALID_VALUE :
            {
                Error = "AL_INVALID_VALUE";
                Description = "a numeric argument is out of range";
                break;
            }

            case AL_INVALID_OPERATION :
            {
                Error = "AL_INVALID_OPERATION";
                Description = "the specified operation is not allowed in the current state";
                break;
            }

            case AL_OUT_OF_MEMORY :
            {
                Error = "AL_OUT_OF_MEMORY";
                Description = "there is not enough memory left to execute the command";
                break;
            }
        }

        // Log the error
		MessageError("ALCheckError", "An internal OpenAL call failed in %s ( %d ) : %s, %s.", File, Line, Error, Description);
    }
}

////////////////////////////////////////////////////////////
/// Set up the audio device
////////////////////////////////////////////////////////////
void AudioInitializeDevice()
{
    // Create the device
    AudioDevice = alcOpenDevice(NULL);

    if (AudioDevice)
    {
        // Create the context
        AudioContext = alcCreateContext(AudioDevice, NULL);

        if (AudioContext)
        {
            // Set the context as the current one (we'll only need one)
            alcMakeContextCurrent(AudioContext);
        }
        else
        {
            MessageError("AudioInitializeDevice", "Failed to create the audio context.");
        }
    }
    else
    {
        MessageError("AudioInitializeDevice", "Failed to open the audio device");
    }

	// Print OpenAL features
	OpenALPrintFeatures();
}


////////////////////////////////////////////////////////////
/// Destroy the audio device
////////////////////////////////////////////////////////////
void AudioDestroyDevice()
{
    // Destroy the context
    alcMakeContextCurrent(NULL);
    if (AudioContext)
        alcDestroyContext(AudioContext);

    // Destroy the device
    if (AudioDevice)
        alcCloseDevice(AudioDevice);
}

////////////////////////////////////////////////////////////
/// Check OpenAL extensions
////////////////////////////////////////////////////////////
bool AudioIsExtensionSupported(char * Extension)
{
    if ((strlen(Extension) > 2) && ((Extension[0] == 'A') && (Extension[1] == 'L') && (Extension[2] == 'C')))
        return alcIsExtensionPresent(AudioDevice, Extension) != AL_FALSE;
    else
        return alIsExtensionPresent(Extension) != AL_FALSE;
}

////////////////////////////////////////////////////////////
/// Get chanels count format
////////////////////////////////////////////////////////////
int AudioGetFormatFromChannelsCount(UInt32 ChannelsCount)
{
	// Find the good format according to the number of channels
    switch (ChannelsCount)
    {
        case 1  : return AL_FORMAT_MONO16;
        case 2  : return AL_FORMAT_STEREO16;
        case 4  : return alGetEnumValue("AL_FORMAT_QUAD16");
        case 6  : return alGetEnumValue("AL_FORMAT_51CHN16");
        case 7  : return alGetEnumValue("AL_FORMAT_61CHN16");
        case 8  : return alGetEnumValue("AL_FORMAT_71CHN16");
        default : return 0;
    }
}

////////////////////////////////////////////////////////////
/// Print audio device and OpenAL features
////////////////////////////////////////////////////////////
void OpenALPrintFeatures()
{
	ALCint ALCMajor = 0;
	ALCint ALCMinor = 0;

	// Get version
	alcGetIntegerv(AudioDevice, ALC_MAJOR_VERSION, sizeof(ALCint), &ALCMajor);
	alcGetIntegerv(AudioDevice, ALC_MINOR_VERSION, sizeof(ALCint), &ALCMinor);

    printf("Audio Device:   %s\nOpenAL Version: %d.%d.0\n",
		   (char*)alcGetString(AudioDevice, ALC_ALL_DEVICES_SPECIFIER), ALCMajor, ALCMinor);
}
