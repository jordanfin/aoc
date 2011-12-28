////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Pack.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
///	Package structure
///		* Header
/// 	* Files table
///		* Datas (the files)
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Get the file path depending on the file type with index
////////////////////////////////////////////////////////////
char * PackGetFilePath( char * FilesPath, Int32 FileIndex )
{
	char * FilePath = (char*)(malloc(sizeof(char) * 64));

	switch (FilesPath[5])
	{
		case 's' : // Sounds
			sprintf(FilePath, "%s%d.ogg", FilesPath, FileIndex);
			break;

		case 'g' : // Graphics
			sprintf(FilePath, "%s%d.png", FilesPath, FileIndex);
			break;

		case 'f' : // Fonts
			sprintf(FilePath, "%s%d.ttf", FilesPath, FileIndex);
			break;

		default : // Anyone
			return (char*)0;
	}

	return FilePath;
}

////////////////////////////////////////////////////////////
/// Create a new pack
////////////////////////////////////////////////////////////
bool PackSaveData( char * PackPath, char * FilesPath, Int32 FilesSize )
{
	// Pack header and pack file entry
	Int32  Index;
	Int32  DataOffset = 0;
	struct PackHeader FileHeader;
	struct PackFile FileEntry;
	struct PackFile * FileEntries = (struct PackFile*)(malloc(sizeof(struct PackFile) * FilesSize));

	// Streams to open the files
	FILE * FilePack = NULL;
	FILE * File = NULL;

	// Data buffer
	char Buffer[1];

	// Initialize the file header
    memset(&FileHeader, 0, sizeof(struct PackHeader));	// Set memory to 0
    memcpy(&FileHeader.UniqueID, "#0!CPE\0", 7);		// Set unique id
    memcpy(&FileHeader.Version, "0.0.1\0", 6);			// Set the version
    FileHeader.FileCount = FilesSize;					// Set the number of files to include

	// File indexes
    for (Index = 0; Index < FilesSize; Index++)
    {
		fopen(&File, PackGetFilePath(FilesPath, Index), "rb");

		if ( File )
		{
			// Reset the file entry memory
            memset(&FileEntry, 0, sizeof(struct PackFile));

            // Set the index of the file
            FileEntry.Index = Index;

			// Get the length of the file and the offset
			fseek(File, 0L, SEEK_END);
			FileEntry.Size = ftell(File);
            FileEntry.Offset = 0;

			// Close file
			fclose(File);

            // Add the file entry into vector
            memcpy(&FileEntries[Index], &FileEntry, sizeof(struct PackFile));
		}
		else
		{
			// Remove data from memory
			Delete(FileEntries);
			Delete(FilePack); Delete(File);
			printf("Error when the program tried to open the file: %s.\n", PackGetFilePath(FilesPath, Index));
			return false;
		}

    }

    // Calculate the offset of the files
    DataOffset += sizeof(struct PackHeader);
    DataOffset += FileHeader.FileCount * sizeof(struct PackFile);

    for (Index = 0; Index < FilesSize; Index++)
    {
        FileEntries[Index].Offset = DataOffset;
        DataOffset += FileEntries[Index].Size;
    }

    // Write the pack file
	fopen(&FilePack, PackPath, "wb");

	// Check if the archive exist
	if ( FilePack )
	{
		// Write the header
		fwrite(&FileHeader, sizeof(struct PackHeader), 1, FilePack);

		// Write the file entries table
		for (Index = 0; Index < FilesSize; Index++)
		{
			fwrite(&FileEntries[Index], sizeof(struct PackFile), 1, FilePack);
		}

		// Write the files data
		for (Index = 0; Index < FilesSize; Index++)
		{
			fopen(&File, PackGetFilePath(FilesPath, FileEntries[Index].Index), "rb");

			if ( File )
			{
				// Set to initial position
				fseek(File, 0L, SEEK_SET);

				// Read and write
				while (fread(Buffer, sizeof(char), 1, File))
				{
					fwrite(Buffer, sizeof(char), 1, FilePack);
				}

				// Close the file
				fclose(File);
			}
		}

		// Close the file
		fclose(FilePack);
	}
	else
	{
		// Remove data from memory
		Delete(FileEntries);
		Delete(FilePack); Delete(File);
		printf("Error when the program tried to create the pack: %s.\n", PackPath);
		return false;
	}

	// Remove data from memory
	Delete(FileEntries);
	Delete(FilePack); Delete(File);

	return true;
}

////////////////////////////////////////////////////////////
/// Extract all data from pack
////////////////////////////////////////////////////////////
bool PackExtractData(char * PackPath, char * ExtractPath)
{
	// Stream to open file
	FILE * FilePack;
	UInt32 Index;

	// Pack header and pack file entry
	struct PackHeader FileHeader;

	// Read the header
	fopen(&FilePack, PackPath, "rb");

	if ( FilePack )
	{
		// Ser memory
		memset(&FileHeader, 0, sizeof(struct PackHeader));

		// Write the header
		fread(&FileHeader, sizeof(struct PackHeader), 1, FilePack);

		if (FileHeader.FileCount == 0)
		{
			fclose(FilePack);
			return false;
		}
	}
	else
	{
		return false;
	}

	// File header readed, so close it
	fclose(FilePack);

	for (Index = 0; Index < FileHeader.FileCount; Index++)
	{
		FILE * File;
		UInt8 * Buffer = 0; Int32 Size;

		if (!PackLoadData(PackPath, Index, &Buffer, &Size))
		{
			Delete(Buffer);
			return false;
		}

		fopen(&File, PackGetFilePath(ExtractPath, Index), "wb");

		if ( File )
		{
			// Put the buffer into the pack
			fwrite(Buffer, sizeof(UInt8), Size, File);

			Delete(Buffer);

			// Close archive
			fclose(File);
		}
		else
		{
			Delete(Buffer);
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////
/// Load file data with index to array
////////////////////////////////////////////////////////////
bool PackLoadData( char * PackPath, UInt32 FileIndex, UInt8 ** Data, Int32 * Size )
{
	// Stream to open file
	FILE * File;

	// Check if the archive exist
	fopen(&File, PackPath, "rb");

	if ( File )
	{
		// Pack header and pack file entry
		struct PackHeader FileHeader;
		struct PackFile FileEntry;

		// Ser memory
		memset(&FileHeader, 0, sizeof(struct PackHeader));

		// Write the header
		fread(&FileHeader, sizeof(struct PackHeader), 1, File);

		// Check pack integrity
		if ( (strcmp(FileHeader.UniqueID, "#0!CPE\0") != 0) ||
			 (strcmp(FileHeader.Version, "0.0.1\0") != 0)	||
			 (FileHeader.FileCount < FileIndex) )
		{
			fclose(File); Delete(File);
			return false;
		}

		// Go to the file entry position
		fseek(File, (FileIndex * sizeof(struct PackFile) + sizeof(struct PackHeader)), SEEK_SET);

		// Read file entry data
		fread(&FileEntry, sizeof(struct PackFile), 1, File);

		if (FileEntry.Index != FileIndex)
		{
			fclose(File); Delete(File);
			return false;
		}

		// Go to file data
		fseek(File, FileEntry.Offset, SEEK_SET);

		// Set size
		*Size = FileEntry.Size;

		// Allocate memory
		*Data = (UInt8*)(malloc(*Size));

		// Read the data
		fread(*Data, sizeof(UInt8), *Size, File);

		// Close archive
		fclose(File);

		// Destroy memory data
		Delete(File);

		return true;
	}

	// Destroy memory data
	Delete(File);

	return false;
}
