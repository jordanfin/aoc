////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Open helper method
////////////////////////////////////////////////////////////
bool IOHelperOpen(FILE** pFile, const char * filename, const char * mode)
{
    #undef fopen

	*pFile = fopen(filename, mode);

	return (*pFile != NULL);
}

////////////////////////////////////////////////////////////
// Read field helper method
////////////////////////////////////////////////////////////
void IOHelperReadField(char * Destination, char * Source, char Separator, UInt32 Position)
{
	UInt32 Length, Current, Last = 0, CurPos = 0;

	Length = strlen(Source);

	for (Current = 0; Current < Length; Current++)
	{
		if (Source[Current] == Separator)
		{
			CurPos++;

			if (CurPos == Position)
				Last = Current;

			if (CurPos == (Position + 1))
			{
				if (Last)
					Last++;

				memcpy(&Destination[0], &Source[Last], (Current - Last));
				return;
			}

			Current++;
		}
	}
}
