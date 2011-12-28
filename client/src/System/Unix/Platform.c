////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Platform.h>
#include <unistd.h>
#include <sys/time.h>

////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////
#if defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)
    #define TIME_ACCURATE_CLOCK
    #include <time.h>
#endif

////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////
#ifdef TIME_ACCURATE_CLOCK
static struct timespec StartTicks = {0, 0};
#else
static struct timeval StartTicks = {0, 0};
#endif

static bool Started = false;

////////////////////////////////////////////////////////////
/// Initialize the system time counter
////////////////////////////////////////////////////////////
void SystemTimeInitialize()
{
    #ifdef TIME_ACCURATE_CLOCK
        clock_gettime(CLOCK_MONOTONIC, &StartTicks);
    #else
        gettimeofday(&StartTicks, NULL);
    #endif

    Started = true;
}

////////////////////////////////////////////////////////////
/// Get the current system time
////////////////////////////////////////////////////////////
UInt32 SystemGetTime()
{
    #ifdef TIME_ACCURATE_CLOCK
        struct timespec Time = {0, 0};
    #else
        struct timeval Time = {0, 0};
    #endif

    if (Started == false)
        SystemTimeInitialize();

    #ifdef TIME_ACCURATE_CLOCK
        clock_gettime(CLOCK_MONOTONIC, &Time);

        return (UInt32)(Time.tv_sec - StartTicks.tv_sec) * 1000 + (Time.tv_nsec - StartTicks.tv_nsec) / 1000000;
    #else
        gettimeofday(&Time, NULL);

        return (UInt32)(Time.tv_sec - StartTicks.tv_sec) * 1000 + (Time.tv_usec - StartTicks.tv_usec) / 1000;
    #endif
}


////////////////////////////////////////////////////////////
/// Suspend the execution of the current thread for a specified time
////////////////////////////////////////////////////////////
void SystemSleep(UInt32 Time)
{
    usleep((unsigned long)(Time * 1000));
}
