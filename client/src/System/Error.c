////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Error.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
/// Show a custom msgbox error
////////////////////////////////////////////////////////////
void MessageError(char * Title, char * Message, ...)
{
	// Output string
	char String[0xFF], Error[0xFF];
	FILE * f;

	// Argument list
	va_list ArgList;

	// Initialize the argument list
	va_start(ArgList, Message);

	// sprintf arguments on string
	#if defined(SYSTEM_WINDOWS)
		vsprintf_s(String, 0xFF, Message, ArgList);
	#else
		vsprintf(String, Message, ArgList);
	#endif

	// Destroy argument list
	va_end(ArgList);

	// Show Message
	#if defined(SYSTEM_WINDOWS)

		MessageBox(NULL, String, Title, (MB_ICONWARNING | MB_OK));

	#elif defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

		char Command[1024];

		sprintf(Command, "xmessage -center \"%s\" \n\n\n", String);

		if (fork() == 0)
		{
			close(1); close(2);
			system(Command);
			exit(0);
		}

	#elif defined(SYSTEM_MACOS)

		int Result = NSRunCriticalAlertPanel(@String, @Title, @"OK", NULL, NULL);

		if ( Result = NSAlertErrorReturn )
		{
			NSLog(@"Error in NSAlertMessage.");
		}

	#endif

	// Log
	if (fopen(&f, ERROR_FILEPATH, "w"))
	{
		sprintf(Error, "Error : Section(%s) - Reason : %s\n", Title, String);
		fputs(Error, f);
		fclose(f);
	}
}
