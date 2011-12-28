#ifndef AUDIO_SOUND_H
#define AUDIO_SOUND_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
/// Data structure of sound
////////////////////////////////////////////////////////////
struct Sound
{
	UInt32	Buffer;   		///< OpenAL buffer identifier
	UInt32	Source;			///< OpenAL source identifier
	Int16*	Samples;  		///< Samples buffer
    Int32	SamplesCount;	///< Total number of samples in the file
    Int32	SampleRate;		///< Number of samples per second
    Int32	ChannelsCount;	///< Number of the channels
    char	File[255];		///< File cache
    UInt32	Time;			///< Time cache
};

////////////////////////////////////////////////////////////
/// Enumeration status of sound
////////////////////////////////////////////////////////////
enum SoundState
{
	Stopped,				///< Sound is not playing
	Paused,					///< Sound is paused
	Playing					///< Sound is playing
};

////////////////////////////////////////////////////////////
/// Start or resume playing the sound
///
/// This function starts the sound if it was stopped, resumes
/// it if it was paused, and does nothing it is it already playing.
/// This function uses its own thread so that it doesn't block
/// the rest of the program while the sound is played.
///
////////////////////////////////////////////////////////////
void SoundPlay(struct Sound * SoundData);

////////////////////////////////////////////////////////////
/// Pause the sound
///
/// This function pauses the sound if it was playing,
/// otherwise (sound already paused or stopped) it has no effect.
///
////////////////////////////////////////////////////////////
void SoundPause(struct Sound * SoundData);

////////////////////////////////////////////////////////////
/// Stop playing the sound
///
/// This function stops the sound if it was playing or paused,
/// and does nothing if it was already stopped.
/// It also resets the playing position (unlike Pause()).
///
////////////////////////////////////////////////////////////
void SoundStop(struct Sound * SoundData);

////////////////////////////////////////////////////////////
/// Set whether or not the sound should loop after reaching the end
///
/// If set, the sound will restart from beginning after
/// reaching the end and so on, until it is stopped or
/// SoundLoop(Sound, false) is called.
/// The default looping state for sound is false.
///
/// \param Loop : true to play in loop, false to play once
///
////////////////////////////////////////////////////////////
void SoundLoop(struct Sound * SoundData, bool Loop);

////////////////////////////////////////////////////////////
/// Set the pitch of the sound
///
/// The pitch represents the perceived fundamental frequency
/// of a sound; thus you can make a sound more acute or grave
/// by changing its pitch. A side effect of changing the pitch
/// is to modify the playing speed of the sound as well.
/// The default value for the pitch is 1.
///
/// \param Pitch : New pitch to apply to the sound
///
////////////////////////////////////////////////////////////
void SoundPitch(struct Sound * SoundData, float Pitch);

////////////////////////////////////////////////////////////
/// Set the volume of the sound
///
/// The volume is a value between 0 (mute) and 100 (full volume).
/// The default value for the volume is 100.
///
/// \param Volume : Volume of the sound
///
////////////////////////////////////////////////////////////
void SoundVolume(struct Sound * SoundData, float Volume);

////////////////////////////////////////////////////////////
/// Set the 3D position of the sound in the audio scene
///
/// Only sounds with one channel (mono sounds) can be
/// spatialized.
/// The default position of a sound is (0, 0, 0).
///
/// \param x : X coordinate of the position of the sound in the scene
/// \param y : Y coordinate of the position of the sound in the scene
/// \param z : Z coordinate of the position of the sound in the scene
///
////////////////////////////////////////////////////////////
void SoundPosition(struct Sound * SoundData, float x, float y, float z);

////////////////////////////////////////////////////////////
/// Make the sound's position relative to the listener or absolute
///
/// Making a sound relative to the listener will ensure that it will always
/// be played the same way regardless the position of the listener.
/// This can be useful for non-spatialized sounds, sounds that are
/// produced by the listener, or sounds attached to it.
/// The default value is false (position is absolute).
///
/// \param Relative : true to set the position relative, true to set it absolute
///
////////////////////////////////////////////////////////////
void SoundRelativeToListener(struct Sound * SoundData, bool Relative);

////////////////////////////////////////////////////////////
/// Set the minimum distance of the sound
///
/// The "minimum distance" of a sound is the maximum
/// distance at which it is heard at its maximum volume. Further
/// than the minimum distance, it will start to fade out according
/// to its attenuation factor. A value of 0 ("inside the head
/// of the listener") is an invalid value and is forbidden.
/// The default value of the minimum distance is 1.
///
/// \param Distance : New minimum distance of the sound
///
////////////////////////////////////////////////////////////
void SoundMinDistance(struct Sound * SoundData, float Distance);

////////////////////////////////////////////////////////////
/// Set the attenuation factor of the sound
///
/// The attenuation is a multiplicative factor which makes
/// the sound more or less loud according to its distance
/// from the listener. An attenuation of 0 will produce a
/// non-attenuated sound, i.e. its volume will always be the same
/// whether it is heard from near or from far. On the other hand,
/// an attenuation value such as 100 will make the sound fade out
/// very quickly as it gets further from the listener.
/// The default value of the attenuation is 1.
///
/// \param Attenuation : New attenuation factor of the sound
///
////////////////////////////////////////////////////////////
void SoundAttenuation(struct Sound * SoundData, float Attenuation);

////////////////////////////////////////////////////////////
/// Get the current status of the sound (stopped, paused, playing)
///
/// \return Current status of the sound
///
////////////////////////////////////////////////////////////
enum SoundState SoundStatus(struct Sound * SoundData);

#endif // AUDIO_SOUND_H
