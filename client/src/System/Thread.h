#ifndef SYSTEM_THREAD_H
#define SYSTEM_THREAD_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>


#if defined(SYSTEM_WINDOWS)

    #include <windows.h>

#elif defined(SYSTEM_LINUX) || defined(SYSTEM_MACOS) || defined(SYSTEM_FREEBSD)

    #include <pthread.h>

#endif

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////

typedef void (*FuncType)(void *);

#if defined(SYSTEM_WINDOWS)

	typedef struct {
		HANDLE		Handle;		///< Win32 thread handle
		FuncType	Function;	///< Function to call as the thread entry point
		void *		Data;		///< Data to pass to the thread function
	} Thread;

#elif defined(SYSTEM_LINUX) || defined(SYSTEM_MACOS) || defined(SYSTEM_FREEBSD)

	typedef struct {
		pthread_t	pThread;	///< Unix thread instance
		bool		IsActive;	///< Thread state (active or inactive)
		FuncType	Function;	///< Function to call as the thread entry point
		void *		Data;		///< Data to pass to the thread function
	} Thread;

#endif

////////////////////////////////////////////////////////////
/// Create and run the thread
///
////////////////////////////////////////////////////////////
void ThreadLaunch(Thread * ThreadEx, FuncType Function, void * UserData);

////////////////////////////////////////////////////////////
/// Wait until the thread finishes
///
////////////////////////////////////////////////////////////
void ThreadWait(Thread * ThreadEx);

////////////////////////////////////////////////////////////
/// Terminate the thread
/// Terminating a thread with this function is not safe,
/// you should rather try to make the thread function
/// terminate by itself
///
////////////////////////////////////////////////////////////
void ThreadTerminate(Thread * ThreadEx);

#endif // SYSTEM_THREAD_H
