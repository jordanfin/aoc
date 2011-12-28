////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Platform.h>
#include <windows.h>

////////////////////////////////////////////////////////////
/// Get the current system time
////////////////////////////////////////////////////////////
double SystemGetTime()
{
    static LARGE_INTEGER Frequency;
    static bool          UseHighPerformanceTimer = 0;

    UseHighPerformanceTimer = QueryPerformanceFrequency(&Frequency);

    if (UseHighPerformanceTimer)
    {
        // High performance counter available : use it
        LARGE_INTEGER CurrentTime;
        QueryPerformanceCounter(&CurrentTime);

        return (double)(CurrentTime.QuadPart) / Frequency.QuadPart;
    }
    else
    {
        // High performance counter not available : use GetTickCount (less accurate)
        return GetTickCount() * 0.001;
    }
}


////////////////////////////////////////////////////////////
/// Suspend the execution of the current thread for a specified time
////////////////////////////////////////////////////////////
void SystemSleep(float Time)
{
	Sleep((DWORD)(Time * 1000));
}
