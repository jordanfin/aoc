////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Audio/SoundLoader.h>
#include <Audio/OpenALHelper.h>
#include <sndfile.h>
#include <System/Error.h>
#include <System/Platform.h>

////////////////////////////////////////////////////////////
/// Open a sound file for reading
////////////////////////////////////////////////////////////
bool SoundFileOpenReadFromFile(struct Sound * SoundData, char * Filename)
{
	// File descriptor
    SNDFILE* FileDescriptor;

	// Size from buffer
	ALsizei Size;

    // Open the sound file
    SF_INFO FileInfo;
    FileDescriptor = sf_open(Filename, SFM_READ, &FileInfo);

    if (!FileDescriptor)
    {
    	MessageError("SoundFileOpenReadFromFile", "Failed to read sound file \" %s \" (%s)", Filename, sf_strerror(FileDescriptor));
        return false;
    }

    // Generate OpenAL Buffers
    ALCheck(alGenBuffers(1, &SoundData->Buffer));

    // Set the sound parameters
    SoundData->ChannelsCount	= FileInfo.channels;
    SoundData->SampleRate		= FileInfo.samplerate;
    SoundData->SamplesCount 	= (Int32)(FileInfo.frames) * SoundData->ChannelsCount;
    SoundData->Time				= SystemGetTime();

	strcpy(SoundData->File, Filename);

	// Read the samples from the opened file
	SoundData->Samples = (Int16*)(malloc(sizeof(Int16) * SoundData->SamplesCount));

	if (sf_read_short(FileDescriptor, &SoundData->Samples[0], SoundData->SamplesCount) == SoundData->SamplesCount)
	{
		// Find the good format according to the number of channels
		ALenum SoundFormat = AudioGetFormatFromChannelsCount(SoundData->ChannelsCount);

		// Check if the format is valid
		if (SoundFormat == 0)
		{
			MessageError("SoundFileOpenReadFromFile", "Unsupported number of channels (%d).", SoundData->ChannelsCount);

			// Close file
			sf_close(FileDescriptor);

			// Return error
			return false;
		}

		// Fill the buffer
		Size = (ALsizei)((SoundData->SamplesCount) * sizeof(Int16));
		ALCheck(alBufferData(SoundData->Buffer, SoundFormat, &SoundData->Samples[0], Size, SoundData->SampleRate));

		// Generate OpenAL Sources
		ALCheck(alGenSources(1, &SoundData->Source));

		// Assign and use the new buffer
		ALCheck(alSourcei(SoundData->Source, AL_BUFFER, SoundData->Buffer));
	}
	else
	{
		// Close file
		sf_close(FileDescriptor);

		// Return error
		return false;
	}

    // Close file
    sf_close(FileDescriptor);

    return true;
}

////////////////////////////////////////////////////////////
/// Open a sound file in memory for reading
////////////////////////////////////////////////////////////
bool SoundFileOpenReadFromMemory(struct Sound * SoundData, void* Data, Int32 SizeInBytes)
{
	// File descriptor
    /*SNDFILE* SoundFile;

    // Prepare the memory I/O structure
    struct SF_VIRTUAL_IO IO = myMemoryIO.Prepare(Data, SizeInBytes);

    // Open the sound file
    SF_INFO FileInfo;
    SoundFile = sf_open_virtual(&IO, SFM_READ, &FileInfo, &myMemoryIO);

    if (!SoundFile)
    {
    	MessageError("SoundFileOpenReadFromMemory", "Failed to read sound file from memory (%s).", sf_strerror(SoundFile));
        return NULL;
    }

    // Set the sound parameters
    //ChannelsCount = FileInfo.channels;
    //SampleRate    = FileInfo.samplerate;
    //NbSamples     = (Int32)(FileInfo.frames) * ChannelsCount;

    return SoundFile;*/

    return true;
}
