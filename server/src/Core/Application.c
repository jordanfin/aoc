////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Core/Application.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
Mutex	AppMutex;		///< Application mutex
bool	AppRun;			///< Application switch

////////////////////////////////////////////////////////////
/// Initialize the application
////////////////////////////////////////////////////////////
bool ApplicationInitialize()
{
	// Initialize mutex
	MutexInitialize(&AppMutex);

	// Start it
	AppSetRun(false);

	// Platform Initialize
	#ifdef SYSTEM_MACOS
		InitializeWorkingDirectory(); // Specific in MacOS
	#endif

	if (!UsersInitialize())
		return false;

	// Initialize database
	if (!DataBaseInitialize())
		return false;

	// Initialize protocol
	if (!ProtocolInitialize(12993))
		return false;

	// Initialize console
	if (!ConsoleInitialize())
		return false;

	// Run it
	AppSetRun(true);

	return true;
}

////////////////////////////////////////////////////////////
/// Run the application
////////////////////////////////////////////////////////////
void ApplicationRun()
{
	while (AppGetRun())
	{

		#ifdef DEBUG
			Sleep(10);
		#endif
	}
}

////////////////////////////////////////////////////////////
/// Destroy the application
////////////////////////////////////////////////////////////
bool ApplicationDestroy()
{
	// Destroy console
	ConsoleDestroy();

	// Destroy protocol
	ProtocolDestroy();

	// Initialize database
	if (!DataBaseDestroy())
		return false;
	
	return true;
}

////////////////////////////////////////////////////////////
/// Set the state of the application
////////////////////////////////////////////////////////////
void AppSetRun(bool value)
{
	MutexLock(&AppMutex);
	AppRun = value;
	MutexUnlock(&AppMutex);
}

////////////////////////////////////////////////////////////
/// Get the state of off the application
////////////////////////////////////////////////////////////
bool AppGetRun()
{
	bool value;

	MutexLock(&AppMutex);
	value = AppRun;
	MutexUnlock(&AppMutex);

	return value;
}
