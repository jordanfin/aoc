////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Thread.h>
#include <stdio.h>

void * ThreadFunc(void * UserData);

////////////////////////////////////////////////////////////
/// Create and run the thread
////////////////////////////////////////////////////////////
void ThreadLaunch(Thread * ThreadEx, FuncType Function, void * UserData)
{
    Int32 Error;

	// Set data from thread
	ThreadEx->Function  = Function;
	ThreadEx->Data      = UserData;

    // Create the thread
    ThreadEx->IsActive = true;
    Error = pthread_create(&ThreadEx->pThread, NULL, &ThreadFunc, ThreadEx);

    // Error ?
    if (Error != 0)
    {
        MessageError("ThreadLaunch", "Failed to create thread");
        ThreadEx->IsActive = false;
    }
}


////////////////////////////////////////////////////////////
/// Wait until the thread finishes
////////////////////////////////////////////////////////////
void ThreadWait(Thread * ThreadEx)
{
    if (ThreadEx->IsActive)
    {
        // Wait for the thread to finish, no timeout
        pthread_join(ThreadEx->pThread, NULL);

        // Reset the thread state
        ThreadEx->IsActive = false;
    }
}


////////////////////////////////////////////////////////////
/// Terminate the thread
/// Terminating a thread with this function is not safe,
/// you should rather try to make the thread function
/// terminate by itself
////////////////////////////////////////////////////////////
void ThreadTerminate(Thread * ThreadEx)
{
    if (ThreadEx->IsActive)
    {
        pthread_cancel(ThreadEx->pThread);
        ThreadEx->IsActive = false;
    }
}


////////////////////////////////////////////////////////////
/// Actual thread entry point, dispatches to instances
////////////////////////////////////////////////////////////
void * ThreadFunc(void * UserData)
{
    // The sfThread instance is stored in the user data
    Thread * ThreadToRun = (Thread*)(UserData);

    // Tell the thread to handle cancel requests immediatly
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // Forward to the instance
    if (ThreadToRun->Function)
        ThreadToRun->Function(ThreadToRun->Data);

    return NULL;
}
