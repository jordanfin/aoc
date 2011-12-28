////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Platform.h>

////////////////////////////////////////////////////////////
/// Get the current system time
////////////////////////////////////////////////////////////
UInt32 SystemGetTime()
{
    static LARGE_INTEGER Frequency;
    static BOOL          UseHighPerformanceTimer = 0;

    UseHighPerformanceTimer = QueryPerformanceFrequency(&Frequency);

    if (UseHighPerformanceTimer)
    {
        // High performance counter available : use it
        LARGE_INTEGER CurrentTime;
        QueryPerformanceCounter(&CurrentTime);

        return (UInt32)(CurrentTime.QuadPart * 1000 / Frequency.QuadPart);
    }
    else
    {
        // High performance counter not available : use GetTickCount (less accurate)
        return (UInt32)(GetTickCount() * 0.001);
    }
}


////////////////////////////////////////////////////////////
/// Suspend the execution of the current thread for a specified time
////////////////////////////////////////////////////////////
void SystemSleep(UInt32 Time)
{
	Sleep((DWORD)(Time));
}

////////////////////////////////////////////////////////////
/// Assign the current thread to one processor
///
/// Based on the DXUTSetProcessorAffinity() function
/// in the DXUT framework
////////////////////////////////////////////////////////////
void SystemSetProcessorAffinity()
{
    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;
    HANDLE hCurrentProcess = GetCurrentProcess();

    if (!GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask))
        return;

    if (dwProcessAffinityMask)
    {
        // Find the lowest processor that our process is allowed to run against
        DWORD_PTR dwAffinityMask = (dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1));

        // Set this as the processor that our thread must always run against
        // This must be a subset of the process affinity mask

        HANDLE hCurrentThread = GetCurrentThread();

        if (hCurrentThread != INVALID_HANDLE_VALUE)
        {
            SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
            CloseHandle(hCurrentThread);
        }
    }

    CloseHandle(hCurrentProcess);
}