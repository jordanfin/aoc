////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Mutex.h>

////////////////////////////////////////////////////////////
/// Default initializer
////////////////////////////////////////////////////////////
void MutexInitialize(Mutex * MutexEx)
{
    InitializeCriticalSection(MutexEx);
}


////////////////////////////////////////////////////////////
/// Mutex destructor
////////////////////////////////////////////////////////////
void MutexDestroy(Mutex * MutexEx)
{
    DeleteCriticalSection(MutexEx);
}


////////////////////////////////////////////////////////////
/// Lock the mutex
////////////////////////////////////////////////////////////
void MutexLock(Mutex * MutexEx)
{
    EnterCriticalSection(MutexEx);
}


////////////////////////////////////////////////////////////
/// Unlock the mutex
////////////////////////////////////////////////////////////
void MutexUnlock(Mutex * MutexEx)
{
    LeaveCriticalSection(MutexEx);
}

////////////////////////////////////////////////////////////
/// Set a variable safely
////////////////////////////////////////////////////////////
void MutexSetVar(Mutex * MutexEx, void * Data, void * Value, UInt32 Lenght)
{
	MutexLock(MutexEx);

	memcpy(Data, Value, Lenght);

	MutexUnlock(MutexEx);
}
