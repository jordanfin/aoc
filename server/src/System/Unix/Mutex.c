////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Mutex.h>

////////////////////////////////////////////////////////////
/// Default initializer
////////////////////////////////////////////////////////////
void MutexInitialize(Mutex* MutexEx)
{
    pthread_mutex_init(&MutexEx, NULL);
}


////////////////////////////////////////////////////////////
/// Mutex destructor
////////////////////////////////////////////////////////////
void MutexDestroy(Mutex* MutexEx)
{
    pthread_mutex_destroy(&MutexEx);
}


////////////////////////////////////////////////////////////
/// Lock the mutex
////////////////////////////////////////////////////////////
void MutexLock(Mutex* MutexEx)
{
    pthread_mutex_lock(&MutexEx);
}


////////////////////////////////////////////////////////////
/// Unlock the mutex
////////////////////////////////////////////////////////////
void MutexUnlock(Mutex* MutexEx)
{
    pthread_mutex_unlock(&MutexEx);
}