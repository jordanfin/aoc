////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Audio/SoundManager.h>
#include <Audio/OpenALHelper.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Sound	SoundData[SOUND_BUFFER_SIZE] = {{0}};	///< Sound buffer array
#ifdef DEBUG
	float 			SoundGeneralVolume = 0;			///< Sound volume (0 to 100)
#else
	float 			SoundGeneralVolume = 100;			///< Sound volume (0 to 100)
#endif

////////////////////////////////////////////////////////////
/// Destroy all sound data and buffer
////////////////////////////////////////////////////////////
void SoundBufferDestroy()
{
	Int32 i = 0;

	for (i = 0; i < SOUND_BUFFER_SIZE; i++)
	{
		if (SoundData[i].Buffer != 0)
		{
			// Stop sound
			SoundStop(&SoundData[i]);

			// Delete source
			ALCheck(alDeleteSources(1, &SoundData[i].Source));

			// Delete buffer
			ALCheck(alDeleteBuffers(1, &SoundData[i].Buffer));

			// Free data
			Delete(SoundData[i].Samples);
		}
	}
}

////////////////////////////////////////////////////////////
/// Play sound from file into buffer
////////////////////////////////////////////////////////////
void SoundBufferPlay(char * File, bool Looping, float x, float y, float z)
{
	struct Sound SoundStruct = { 0 };

	SoundBufferLoad(&SoundStruct, File);

	if (SoundStruct.Buffer != 0)
	{
		if (SoundStatus(&SoundStruct) == Stopped)
		{
			// Set volume
			SoundVolume(&SoundStruct, SoundGeneralVolume);

			// Set position
			SoundPosition(&SoundStruct, x, y, z);

			// Set looping
			SoundLoop(&SoundStruct, Looping);

			// Play sound
			SoundPlay(&SoundStruct);
		}
	}
}

////////////////////////////////////////////////////////////
/// Load sound into buffer
////////////////////////////////////////////////////////////
void SoundBufferLoad(struct Sound * SoundStruct, char * File)
{
	UInt32 MinorTime = 0; // Minor time means the most old file
	UInt32 Dimension = 0; // Dimension found
	Int32 i = 0;

	// Search if the sound is loaded
	for (i = 0; i < SOUND_BUFFER_SIZE; i++)
	{
		if (SoundData[i].File != NULL)
		{
			if (strcmp(SoundData[i].File, File) == 0)
			{
				// The sound is loaded.. just play it
				SoundStruct = &SoundData[i];
				return;
			}
		}
	}

	// Search free sound
	for (i = 0; i < SOUND_BUFFER_SIZE; i++)
	{
		if (SoundData[i].Buffer == 0)
		{
			// Load sound
			if (SOUND_FROM_MEMORY)
			{
				char * BufferData;
				UInt32 BufferSize;

				// Load buffer from pack
				// if (!PackLoadData(File, BufferData, &BufferSize))
				// {
				//	return;
				// }

				// Load buffer data from memory
				if (SoundFileOpenReadFromMemory(SoundStruct, BufferData, BufferSize))
				{
					// Destroy data
					Delete(BufferData);
					return;
				}
			}
			else
			{
				if (SoundFileOpenReadFromFile(SoundStruct, File))
				{
					strcpy(SoundStruct->File, File);
					return;
				}
			}
		}
	}

	// Search last used
	for (i = 0; i < SOUND_BUFFER_SIZE; i++)
	{
		if (SoundStatus(&SoundData[i]) == Stopped)
		{
			if (SoundData[i].Time < MinorTime)
			{
				MinorTime = SoundData[i].Time;
				Dimension = i;
			}
		}
	}

	if (MinorTime != 0)
	{
		if (SoundData[Dimension].Buffer != 0)
		{
			// Remove data from the previous sound
			ALCheck(alDeleteBuffers(1, &SoundData[Dimension].Buffer));

			// Delete source
			ALCheck(alDeleteSources(1, &SoundData[Dimension].Source));
		}

		// Return last used
		SoundStruct = &SoundData[Dimension];
	}
	else
	{
		// There isn't sounds available
		SoundStruct = NULL;
	}
}

////////////////////////////////////////////////////////////
/// Stop all sounds from buffer
////////////////////////////////////////////////////////////
void SoundBufferStop()
{
	Int32 i = 0;

	for (i = 0; i < SOUND_BUFFER_SIZE; i++)
	{
		if (SoundData[i].Buffer != 0)
		{
			// Stop sound
			SoundStop(&SoundData[i]);
		}
	}
}

////////////////////////////////////////////////////////////
/// Update sounds volume
////////////////////////////////////////////////////////////
void SoundBufferVolume(float Volume)
{
	Int32 i = 0;

	for (i = 0; i < SOUND_BUFFER_SIZE; i++)
	{
		if (SoundData[i].Buffer != 0)
		{
			// Set volume sound
			SoundVolume(&SoundData[i], Volume);
		}
	}

	SoundGeneralVolume = Volume;
}

////////////////////////////////////////////////////////////
/// Move the listener position
////////////////////////////////////////////////////////////
void SoundBufferMoveListener(float x, float y, float z)
{

}
