////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Audio/Sound.h>
#include <Audio/OpenALHelper.h>

////////////////////////////////////////////////////////////
/// Start or resume playing the sound
////////////////////////////////////////////////////////////
void SoundPlay(struct Sound * SoundData)
{
    ALCheck(alSourcePlay(SoundData->Source));
}

////////////////////////////////////////////////////////////
/// Pause the sound
////////////////////////////////////////////////////////////
void SoundPause(struct Sound * SoundData)
{
    ALCheck(alSourcePause(SoundData->Source));
}

////////////////////////////////////////////////////////////
/// Stop playing the sound
////////////////////////////////////////////////////////////
void SoundStop(struct Sound * SoundData)
{
    ALCheck(alSourceStop(SoundData->Source));
}

////////////////////////////////////////////////////////////
/// Set whether or not the sound should loop after reaching the end
////////////////////////////////////////////////////////////
void SoundLoop(struct Sound * SoundData, bool Loop)
{
	// Set loop
    ALCheck(alSourcei(SoundData->Source, AL_LOOPING, Loop));
}

////////////////////////////////////////////////////////////
/// Set the pitch of the sound
////////////////////////////////////////////////////////////
void SoundPitch(struct Sound * SoundData, float Pitch)
{
	// Set frequency
    ALCheck(alSourcef(SoundData->Source, AL_PITCH, Pitch));
}

////////////////////////////////////////////////////////////
/// Set the volume of the sound
////////////////////////////////////////////////////////////
void SoundVolume(struct Sound * SoundData, float Volume)
{
	// Set volume
    ALCheck(alSourcef(SoundData->Source, AL_GAIN, Volume * 0.01f));
}

////////////////////////////////////////////////////////////
/// Set the 3D position of the sound in the audio scene
////////////////////////////////////////////////////////////
void SoundPosition(struct Sound * SoundData, float x, float y, float z)
{
	// Set 3D sound
    ALCheck(alSource3f(SoundData->Source, AL_POSITION, x, y, z));
}

////////////////////////////////////////////////////////////
/// Make the sound's position relative to the listener or absolute
////////////////////////////////////////////////////////////
void SoundRelativeToListener(struct Sound * SoundData, bool Relative)
{
	// Relative sound or absolute
    ALCheck(alSourcei(SoundData->Source, AL_SOURCE_RELATIVE, Relative));
}

////////////////////////////////////////////////////////////
/// Set the minimum distance of the sound
////////////////////////////////////////////////////////////
void SoundMinDistance(struct Sound * SoundData, float Distance)
{
    ALCheck(alSourcef(SoundData->Source, AL_REFERENCE_DISTANCE, Distance));
}

////////////////////////////////////////////////////////////
/// Set the attenuation factor of the sound
////////////////////////////////////////////////////////////
void SoundAttenuation(struct Sound * SoundData, float Attenuation)
{
    ALCheck(alSourcef(SoundData->Source, AL_ROLLOFF_FACTOR, Attenuation));
}

////////////////////////////////////////////////////////////
/// Get the current status of the sound (stopped, paused, playing)
////////////////////////////////////////////////////////////
enum SoundState SoundStatus(struct Sound * SoundData)
{
    ALint Status;
    ALCheck(alGetSourcei(SoundData->Source, AL_SOURCE_STATE, &Status));

    switch (Status)
    {
        case AL_INITIAL :
        case AL_STOPPED : return Stopped;
        case AL_PAUSED :  return Paused;
        case AL_PLAYING : return Playing;
    }

    return Stopped;
}
