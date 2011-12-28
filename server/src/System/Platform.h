#ifndef SYSTEM_PLATFORM_H
#define SYSTEM_PLATFORM_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
/// Get the current system time
///
/// return System time, in seconds
///
////////////////////////////////////////////////////////////
double SystemGetTime();

////////////////////////////////////////////////////////////
/// Suspend the execution of the current thread for a specified time
///
/// param Time : Time to sleep, in seconds
///
////////////////////////////////////////////////////////////
void SystemSleep(float Time);

#ifdef SYSTEM_MACOS

#include <CoreFoundation/CoreFoundation.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
/// Under Mac OS X, when launching an application from the Finder,
/// the default working directory is the user home directory ;
/// when launching from Xcode, the default one is the directory
/// containing the application. In order to produce a uniform behaviour
/// and simplify the use of resources, sets the working directory to
/// the Resources folder of the application bundle.
/// The "constructor" attribute forces the function to be called
/// at library loading time.
////////////////////////////////////////////////////////////
void InitializeWorkingDirectory(void)
{
	char PathBuffer[4096];
	bool Encoded = false;

	// Get the application bundle
	CFBundleRef MainBundle = CFBundleGetMainBundle();
	assert(MainBundle != NULL);

	// Get the resource directory URL
	CFURLRef ResourceDirectory = CFBundleCopyResourcesDirectoryURL(MainBundle);
	assert(ResourceDirectory != NULL);

	// Convert it as absolute URL
	CFURLRef AbsoluteURL = CFURLCopyAbsoluteURL(ResourceDirectory);
	assert(AbsoluteURL != NULL);

	// Get the path as C string
	Encoded = CFURLGetFileSystemRepresentation(AbsoluteURL, true, (UInt8 *)PathBuffer, 4096);
	assert(Encoded);

	// Set the working directory
	chdir(PathBuffer);

	CFRelease(AbsoluteURL);
	CFRelease(ResourceDirectory);
}

#endif // SYSTEM_MACOS

#endif // SYSTEM_PLATFORM_H
