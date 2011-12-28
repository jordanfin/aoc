////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/IOHelper.h>
#include <System/Win32/vsscanf.win.c>

////////////////////////////////////////////////////////////
// String print helper method
////////////////////////////////////////////////////////////
void IOHelperStrPrint(char * str, const char * format, ...)
{
	// Argument list
	va_list ArgList;

	// Initialize the argument list
	va_start(ArgList, format);

	// sprintf arguments on string
	vsprintf_s(str, _vscprintf( format, ArgList ) + 1, format, ArgList);

	// Destroy argument list
	va_end(ArgList);
}

////////////////////////////////////////////////////////////
// String copy helper method
////////////////////////////////////////////////////////////
void IOHelperStrCpy(char * destination, const char * source)
{
    if (destination)
		strcpy_s(destination, strlen(source) + 1, source);
}

////////////////////////////////////////////////////////////
// Sscan helper method
////////////////////////////////////////////////////////////
Int32 IOHelperSscan(const char * str, const char * format, ...)
{
	// Argument list
	va_list ArgList;
	Int32	Count;

	// Initialize the argument list
	va_start(ArgList, format);

	// Scan
	Count = vsscanf(str, format, ArgList);

	// Destroy argument list
	va_end(ArgList);

	return Count;
}

////////////////////////////////////////////////////////////
// Open helper method
////////////////////////////////////////////////////////////
bool IOHelperOpen(FILE** pFile, const char * filename, const char * mode)
{
	fopen_s(pFile, filename, mode);

	if (!*pFile)
		return false;

	return true;
}

////////////////////////////////////////////////////////////
// Read field helper method
////////////////////////////////////////////////////////////
void IOHelperReadField(char * Destination, char * Source, char Separator, UInt32 Position)
{
	UInt32 Count, Last = 0, Current = 0, SepCount = 0, Length = strlen(Source);

	for (Count = 0; Count < Length; Count++)
	{
		if (Source[Count] == Separator)
		{
			SepCount++;

			if (SepCount == Position)
				Last = Count;
		}

		if (SepCount == (Position + 1) || Count == (Length - 1))
		{
			Current = Count;
			break;
		}
	}

	if (Position == 0)
	{
		memcpy(&Destination[0], &Source[Last], Current - Last);
		Destination[Current - Last] = NULLCHAR;
	}
	else
	{
		if (Current == (Length - 1))
		{
			memcpy(&Destination[0], &Source[Last + 1], Current - Last);
			Destination[Current - Last] = NULLCHAR;
		}
		else
		{
			memcpy(&Destination[0], &Source[Last + 1], Current - Last - 1);
			Destination[Current - Last - 1] = NULLCHAR;
		}		
	}
}