////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Timer.h>
#include <System/Platform.h>

////////////////////////////////////////////////////////////
// Reset the timer to 0
////////////////////////////////////////////////////////////
void TimerReset(struct Timer * TimerData)
{
	// Update elapsed time
	TimerData->ElapsedTime = SystemGetTime();

	// Reset
	TimerData->LastCounter = TimerData->ElapsedTime;
}

////////////////////////////////////////////////////////////
// Check time
////////////////////////////////////////////////////////////
bool TimerCheck(struct Timer * TimerData, UInt32 Time)
{
	// Update elapsed time
	TimerData->ElapsedTime = SystemGetTime();

	if ( TimerData->ElapsedTime - TimerData->LastCounter >= Time )
	{
		TimerReset(TimerData);
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////
// Return time of timer
////////////////////////////////////////////////////////////
UInt32 TimerGetTime(struct Timer * TimerData)
{
	// Update elapsed time
	TimerData->ElapsedTime = SystemGetTime();

	return (TimerData->ElapsedTime - TimerData->LastCounter);
}
