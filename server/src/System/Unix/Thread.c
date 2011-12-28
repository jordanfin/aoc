////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Thread.h>
#include <stdio.h>

void* ThreadFunc(void* UserData);

////////////////////////////////////////////////////////////
/// Create and run the thread
////////////////////////////////////////////////////////////
void ThreadLaunch(Thread* ThreadEx, FuncType Function, void* UserData)
{
	// Set data from thread
	ThreadEx->myFunction = Function;
	ThreadEx->myUserData = UserData;

    // Create the thread
    ThreadEx->myIsActive = true;
    int Error = pthread_create(&ThreadEx->myThread, NULL, &ThreadFunc, ThreadEx);

    // Error ?
    if (Error != 0)
    {
        printf("Failed to create thread");
        ThreadEx->myIsActive = false;
    }
}


////////////////////////////////////////////////////////////
/// Wait until the thread finishes
////////////////////////////////////////////////////////////
void ThreadWait(Thread* ThreadEx)
{
    if (ThreadEx->myIsActive)
    {
        // Wait for the thread to finish, no timeout
        pthread_join(ThreadEx->myThread, NULL);

        // Reset the thread state
        ThreadEx->myIsActive = false;
    }
}


////////////////////////////////////////////////////////////
/// Terminate the thread
/// Terminating a thread with this function is not safe,
/// you should rather try to make the thread function
/// terminate by itself
////////////////////////////////////////////////////////////
void ThreadTerminate(Thread* ThreadEx)
{
    if (ThreadEx->myIsActive)
    {
        pthread_cancel(ThreadEx->myThread);
        ThreadEx->myIsActive = false;
    }
}


////////////////////////////////////////////////////////////
/// Actual thread entry point, dispatches to instances
////////////////////////////////////////////////////////////
void* ThreadFunc(void* UserData)
{
    // The sfThread instance is stored in the user data
    Thread* ThreadToRun = (Thread*)(UserData);

    // Tell the thread to handle cancel requests immediatly
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // Forward to the instance
    if (ThreadToRun->myFunction)
        ThreadToRun->myFunction(ThreadToRun->myUserData);

    return NULL;
}
