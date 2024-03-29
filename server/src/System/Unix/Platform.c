////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Platform.h>
#include <unistd.h>
#include <sys/time.h>

////////////////////////////////////////////////////////////
/// Get the current system time
////////////////////////////////////////////////////////////
double SystemGetTime()
{
    timeval Time = {0, 0};
    gettimeofday(&Time, NULL);

    return Time.tv_sec + Time.tv_usec / 1000000.;
}


////////////////////////////////////////////////////////////
/// Suspend the execution of the current thread for a specified time
////////////////////////////////////////////////////////////
void SystemSleep(float Time)
{
    usleep((unsigned long)(Time * 1000000));
}
