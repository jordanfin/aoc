////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Game/Intro.h>
#include <Window/Window.h>
#include <Graphics/VideoRender.h>
#include <Audio/SoundManager.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define INTRO_VIDEO_THEME				"data/video/intro.avi"
#define INTRO_SOUND_THEME				"data/audio/2.ogg"

////////////////////////////////////////////////////////////
/// Show the initial video
////////////////////////////////////////////////////////////
void IntroShow()
{
	struct Video VideoIntro;

	VideoLoad(&VideoIntro, INTRO_VIDEO_THEME);
	
	SoundBufferPlay(INTRO_SOUND_THEME, false, 0, 0, 0);

	while (VideoIntro.Loaded && !VideoIntro.Finished)
	{
		// Render it
		VideoRender(&VideoIntro, false, 0, 0, WindowWidth, WindowHeight);

		// Display on window handle
		WindowDisplay();

		// Process input events
		WindowProcessEvents();
	}

	// Stop music
	SoundBufferStop();

	// Delete the video
	VideoDelete(&VideoIntro);
}