#ifndef AUDIO_SOUNDLOADER_H
#define AUDIO_SOUNDLOADER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Audio/Sound.h>

////////////////////////////////////////////////////////////
/// Open a sound file for reading
///
/// \param Filename : Path of the sound file to load
///
/// \return SNDFILE pointer if the file was
///			successfully opened
///
////////////////////////////////////////////////////////////
bool SoundFileOpenReadFromFile(struct Sound * SoundData, char * Filename);

////////////////////////////////////////////////////////////
/// Open a sound file in memory for reading
///
/// \param Data        : Pointer to the file data in memory
/// \param SizeInBytes : Size of the data to load, in bytes
///
/// \return SNDFILE pointer if the file was
///			successfully opened
///
////////////////////////////////////////////////////////////
bool SoundFileOpenReadFromMemory(struct Sound * SoundData, void * Data, Int32 SizeInBytes);

#endif // AUDIO_SOUNDLOADER_H
