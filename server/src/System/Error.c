////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Error.h>

////////////////////////////////////////////////////////////
/// Error console report
////////////////////////////////////////////////////////////
void MessageError(char * Section, char * Message, ...)
{
	// Output string
	char String[0xFF], Error[0xFF];
	FILE * f;

	// Argument list
	va_list ArgList;

	// Initialize the argument list
	va_start(ArgList, Message);

	// sprintf arguments on string
	#ifdef SYSTEM_WINDOWS
		vsprintf_s(String, 0xFF, Message, ArgList);
	#else
		vsprintf(String, Message, ArgList);
	#endif

	// Destroy argument list
	va_end(ArgList);

	// Show Message
	sprintf(Error, "Error : Section(%s) - Reason : %s\n", Section, String);
	printf(Error);

	// Log
	if (IOHelperOpen(&f, ERROR_FILEPATH, "w+"))
	{
		fputs(Error, f);
		fclose(f);
	}
}