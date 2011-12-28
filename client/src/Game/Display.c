////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Game/Display.h>
#include <Graphics/Font.h>
#include <System/Timer.h>

#ifdef DEBUG
#include <Terrain/Terrain.h>
#endif

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Timer TimerData;		///< Timer
UInt32 FrameCounter = 0;	///< Frames per second counter
char   FrameText[32];		///< Char pointer to fps text

#ifdef DEBUG
extern char CameraText[0xFF];
#endif

////////////////////////////////////////////////////////////
/// Initialize user display info
////////////////////////////////////////////////////////////
void DisplayInitialize()
{
	// Reset timer
	TimerReset(&TimerData);
}

////////////////////////////////////////////////////////////
/// Render user display info
////////////////////////////////////////////////////////////
void DisplayUpdate()
{
	// Update
	FrameCounter++;

	if (TimerGetTime(&TimerData) > 1000)
	{
		// Format string
		sprintf(FrameText, "FPS: %d", FrameCounter);

		// Reset frame counter
		FrameCounter = 0;

		// Restart timer
		TimerReset(&TimerData);
	}

	// Render
	FontRenderText(5, 5, FrameText);

	// Render camera info
	#ifdef DEBUG
		FontRenderText(5, 15, CameraText);
	#endif
}
