#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Timer
{
	UInt32 LastCounter;
	UInt32 ElapsedTime;
};

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Reset the timer to 0
////////////////////////////////////////////////////////////
void TimerReset(struct Timer * TimerData);

////////////////////////////////////////////////////////////
// Check time
////////////////////////////////////////////////////////////
bool TimerCheck(struct Timer * TimerData, UInt32 Time);

////////////////////////////////////////////////////////////
// Return time of timer
////////////////////////////////////////////////////////////
UInt32 TimerGetTime(struct Timer * TimerData);

#endif // SYSTEM_TIMER_H
