#ifndef AUDIO_SOUNDMANAGER_H
#define AUDIO_SOUNDMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Audio/SoundLoader.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define SOUND_BUFFER_SIZE				32		///< Max sounds that the sound manager can manage both
#define SOUND_MAX_DISTANCE_TO_SOURCE	150 	///< The distance at which the sound is inaudible
#define SOUND_FROM_MEMORY				0		///< Load sound from pack in memory or not

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Destroy all sound data and buffer
////////////////////////////////////////////////////////////
void SoundBufferDestroy();

////////////////////////////////////////////////////////////
/// Play sound from file into buffer
////////////////////////////////////////////////////////////
void SoundBufferPlay(char * File, bool Looping, float x, float y, float z);

////////////////////////////////////////////////////////////
/// Load sound into buffer
////////////////////////////////////////////////////////////
void SoundBufferLoad(struct Sound * SoundStruct, char * File);

////////////////////////////////////////////////////////////
/// Stop all sounds from buffer
////////////////////////////////////////////////////////////
void SoundBufferStop();

////////////////////////////////////////////////////////////
/// Update sounds volume
////////////////////////////////////////////////////////////
void SoundBufferVolume(float Volume);

////////////////////////////////////////////////////////////
/// Move the listener position
////////////////////////////////////////////////////////////
void SoundBufferMoveListener(float x, float y, float z);

#endif // AUDIO_SOUNDMANAGER_H
