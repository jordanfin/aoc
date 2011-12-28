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
