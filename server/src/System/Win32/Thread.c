////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Thread.h>
#include <process.h>

////////////////////////////////////////////////////////////
/// Actual thread entry point, dispatches to instances
///
/// param UserData : Data to pass to the thread function
///
/// return Error code
///
////////////////////////////////////////////////////////////
unsigned int __stdcall ThreadFunc(void * UserData);

////////////////////////////////////////////////////////////
/// Create and run the thread
////////////////////////////////////////////////////////////
void ThreadLaunch(Thread * ThreadEx, FuncType Function, void * UserData)
{
	// Set data from thread
	ThreadEx->myFunction = Function;
	ThreadEx->myUserData = UserData;

    // Create the thread
    ThreadEx->myHandle = (HANDLE)(_beginthreadex(NULL, 0, &ThreadFunc, ThreadEx, 0, NULL));

    // Error ?
    if (ThreadEx->myHandle == NULL)
        MessageError("ThreadLaunch", "Failed to create thread.");
}


////////////////////////////////////////////////////////////
/// Wait until the thread finishes
////////////////////////////////////////////////////////////
void ThreadWait(Thread * ThreadEx)
{
    if (ThreadEx->myHandle)
    {
        // Wait for the thread to finish, no timeout
        WaitForSingleObject(ThreadEx->myHandle, INFINITE);

        // Don't forget to close the thread handle (__endthreadex doesn't do it)
        CloseHandle(ThreadEx->myHandle);
        ThreadEx->myHandle = NULL;
    }
}

////////////////////////////////////////////////////////////
/// Sleep the thread
////////////////////////////////////////////////////////////
void ThreadSleep(Thread * ThreadEx, UInt32 Miliseconds)
{
	if (ThreadEx->myHandle)
    {
        // Wait for the thread to finish, no timeout
        WaitForSingleObject(ThreadEx->myHandle, Miliseconds);
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
    if (ThreadEx->myHandle)
    {
        TerminateThread(ThreadEx->myHandle, 0);
        ThreadEx->myHandle = NULL;
    }
}

////////////////////////////////////////////////////////////
/// Actual thread entry point, dispatches to instances
////////////////////////////////////////////////////////////
unsigned int __stdcall ThreadFunc(void * UserData)
{
    // The Thread instance is stored in the user data
    Thread * ThreadInstance = (Thread*)(UserData);

    // Forward to the instance
    if (ThreadInstance->myFunction)
        ThreadInstance->myFunction(ThreadInstance->myUserData);

    // Optional, but it is cleaner
    _endthreadex(0);

    return 0;
}
