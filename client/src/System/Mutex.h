#ifndef SYSTEM_MUTEX_H
#define SYSTEM_MUTEX_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
/// Mutex defines a mutex (MUTual EXclusion) object,
/// that allows a thread to lock critical instructions
/// to avoid simultaneous access with other threads.
/// The Win32 version uses critical sections, as it is
/// faster than mutexes.
////////////////////////////////////////////////////////////

#if defined(SYSTEM_WINDOWS)

	#include <windows.h>

	typedef CRITICAL_SECTION Mutex;

#elif defined(SYSTEM_LINUX) || defined(SYSTEM_MACOS) || defined(SYSTEM_FREEBSD)

	#include <pthread.h>

	typedef pthread_mutex_t Mutex;

	#define MUTEX_INITIALIZE PTHREAD_MUTEX_INITIALIZER

#endif

////////////////////////////////////////////////////////////
/// Mutex initializer
////////////////////////////////////////////////////////////
void MutexInitialize(Mutex * MutexEx);

////////////////////////////////////////////////////////////
/// Mutex destructor
////////////////////////////////////////////////////////////
void MutexDestroy(Mutex * MutexEx);

////////////////////////////////////////////////////////////
/// Lock the mutex
///
////////////////////////////////////////////////////////////
void MutexLock(Mutex * MutexEx);

////////////////////////////////////////////////////////////
/// Unlock the mutex
///
////////////////////////////////////////////////////////////
void MutexUnlock(Mutex * MutexEx);

////////////////////////////////////////////////////////////
/// Set a variable safely
///
////////////////////////////////////////////////////////////
void MutexSetVar(Mutex * MutexEx, void * Data, void * Value, UInt32 Length);

#endif // SYSTEM_MUTEX_H
