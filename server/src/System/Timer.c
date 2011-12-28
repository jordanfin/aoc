////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Timer.h>

////////////////////////////////////////////////////////////
// Reset the timer to 0
////////////////////////////////////////////////////////////
void TimerReset(struct Timer * TimerData)
{
	// Update elapsed time
	TimerData->ElapsedTime = (float)SystemGetTime();
	
	// Reset
	TimerData->LastCounter = TimerData->ElapsedTime;
}

////////////////////////////////////////////////////////////
// Check time (reseting it)
////////////////////////////////////////////////////////////
bool TimerCheckReset(struct Timer * TimerData, float Time)
{
	// Update elapsed time
	TimerData->ElapsedTime = (float)SystemGetTime();

	if ( TimerData->ElapsedTime - TimerData->LastCounter >= Time )
	{
		TimerReset(TimerData);
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////
// Check time
////////////////////////////////////////////////////////////
bool TimerCheck(struct Timer * TimerData, float Time)
{
	// Update elapsed time
	TimerData->ElapsedTime = (float)SystemGetTime();

	if ( TimerData->ElapsedTime - TimerData->LastCounter >= Time )
		return true;

	return false;
}

////////////////////////////////////////////////////////////
// Return time of timer
////////////////////////////////////////////////////////////
float TimerGetTime(struct Timer * TimerData)
{
	// Update elapsed time
	TimerData->ElapsedTime = (float)SystemGetTime();

	return (float)(TimerData->ElapsedTime - TimerData->LastCounter);
}