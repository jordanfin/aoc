////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Game/Application.h>

#include <System/Platform.h>		///< Platform SO includes
#include <System/Thread.h>
#include <System/Mutex.h>
#include <System/CPUInfo.h>

#include <Network/Protocol.h>		///< Network includes

#include <Window/Window.h>			///< Window includes

#include <Graphics/OpenGLHelper.h> 	///< Graphic includes
#include <Graphics/Font.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureManager.h>
#include <Graphics/VideoRender.h>

#include <Audio/OpenALHelper.h> 	///< Audio includes
#include <Audio/SoundManager.h>

#include <Game/Configuration.h>		///< Game includes
#include <Game/Language.h>
#include <Game/Intro.h>
#include <Game/Login.h>
#include <Game/Index.h>
#include <Game/Display.h>
#include <Game/User.h>

#include <Gui/WindowManager.h>		///< GUI includes

#include <Terrain/Terrain.h>		///< Terrain includes

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
UInt32	GameStatus		= GAME_DESTROY;	///< Game status controller
Mutex   GameStatusMutex;                ///< Game status mutex
Thread  GameEventsThread;               ///< Game event update thread

// Terrain test
struct TerrainType * Terrain;

////////////////////////////////////////////////////////////
/// Initialize the application
////////////////////////////////////////////////////////////
bool GameInitialize()
{
	struct VideoMode		DefaultMode;
	struct WindowSettings	Settings;

	// Initialize math library
	MathInitialize();

	// Initialize the game status mutex
	MutexInitialize(&GameStatusMutex);

	// Set status of the game
	GameSetStatus(GAME_INITIALIZE);

	// Initialize the configuration
	ConfigInitialize();

	// Platform Initialize
	#ifdef SYSTEM_MACOS
		InitializeWorkingDirectory(); // Specific in MacOS
	#endif

	// Get the CPU features
	CPUInitialize();

	// Create the window handle
	DefaultMode.Width = 1024;
	DefaultMode.Height = 768;
	DefaultMode.BitsPerPixel = ConfigGet()->BitsPerPixel;

	Settings.DepthBits = 24;
	Settings.StencilBits = 8;
	Settings.AntialiasingLevel = 0;

	if (!WindowCreate(&DefaultMode,
					  APP_NAME,
					  #ifdef DEBUG
						NonStyle,
					  #else
						ConfigGet()->WindowStyle,
					  #endif
					  &Settings))
        return false;

	// Set the processor affinity
	//#ifdef DEBUG
	//	SystemSetProcessorAffinity();
    //#endif

	WindowUseVerticalSync(ConfigGet()->Vsync);

	#ifdef DEBUG
        WindowShowMouseCursor(true);
    #else
        WindowShowMouseCursor(false);
    #endif

	// Initialize OpenGL
	if (!OpenGLInitialize())
		return false;

	// Terrain test
	Terrain = TerrainCreate();

	if (Terrain == NULL)
		return false;

	if (!TerrainInitialize(Terrain, TERRAIN_LOAD_TEXTURE, 0, 0, "data/terrain/heightmap.png"))
		return false;

	// Initialize Fonts
	FontInitialize();

	// Initialize Audio Device
	AudioInitializeDevice();

	if (!IndexInitialize())
		return false;

	// Initialize language
	if (!LanguageInitialize())
		return false;

	// Initialize the display
	DisplayInitialize();

	// Initialize the protocol
	ProtocolInitialize();

	// Initialize the gui
	GuiInitialize();

	// Show intro
	#ifndef DEBUG
		//IntroShow();
	#endif

	// Set status to login
	GameSetStatus(GAME_LOGIN);

    // Launch the event thread
    ThreadLaunch(&GameEventsThread, (FuncType)&GameEvents, NULL);

	return true;
}

////////////////////////////////////////////////////////////
/// Update the events of the application
////////////////////////////////////////////////////////////
void GameEvents()
{
    UInt32 StatusController = GAME_DESTROY;

	do
    {
		struct Event Evt = {{0}};

        // Get the event
        WindowGetEvent(&Evt);

        // Get the game status
        GameGetStatus(&StatusController);

        // Update game status events
		switch (StatusController)
		{
			case GAME_LOGIN :
				if (Evt.Key.Code == Esc)
					GameSetStatus(GAME_DESTROY);
				if (Evt.Key.Code == R)
					GameSetStatus(GAME_RUN);
				break;

			case GAME_ACCOUNT :
				if (Evt.Key.Code == Esc)
					GameSetStatus(GAME_LOGIN);
				break;

			case GAME_RUN :
				if (Evt.Key.Code == Q)
					// GameSetStatus(GAME_ACCOUNT);
					GameSetStatus(GAME_LOGIN);

				TerrainUpdateCameraAngle(Terrain, &Evt, 1.0f);

				TerrainUpdateMovement(Terrain, &Evt, 1.0f);

				if (Evt.Key.Code == F1)
					TextureScreenShot(WindowWidth, WindowHeight);
				break;
		}

        // Compute the user interface
        GuiUpdate(&Evt);

		#if defined(DEBUG)
			SystemSleep(30);
		#endif
    } while (StatusController != GAME_DESTROY);
}

////////////////////////////////////////////////////////////
/// Run the application
////////////////////////////////////////////////////////////
void GameRun()
{
    UInt32 StatusController = GAME_DESTROY;

	do
	{
		// Clear OpenGL scene
		OpenGLClear();

		// Reset the current modelview matrix
		glLoadIdentity();

		// Get the game status
		GameGetStatus(&StatusController);

		// Check keys
		switch (StatusController)
		{
			case GAME_LOGIN :
				OpenGLEnable2D();
				LoginRenderLogo();
				break;

			case GAME_ACCOUNT :
				OpenGLEnable2D();
				LoginRenderLogo();
				break;

			case GAME_RUN :
				// Enable 3D rendering
				// OpenGLEnable3D();

				// Terrain test
				TerrainRenderTest(Terrain);

				OpenGLEnable2D();

				// ...

				break;
		}

		// Render GUI
		GuiRender();

		// Update the display
		DisplayUpdate();

		OpenGLDisable2D();

		// Display on window handle
		WindowDisplay();

		// Process input events
		WindowProcessEvents();

		#if defined(DEBUG)
			SystemSleep(10);
		#endif
	} while (StatusController != GAME_DESTROY);
}

////////////////////////////////////////////////////////////
/// Destroy and cleanup the application
////////////////////////////////////////////////////////////
bool GameDestroy()
{
	// Destroy the login
	LoginClose();

	// Delete the user
	UserDestroy();

	// Save config
	ConfigSave();

	// Destroy Video Device
	VideoDestroy();

	// Destroy GUI
	GuiDestroy();

	// Destroy Fonts
	FontDestroy();

	// Terrain test
	TerrainDestroy(Terrain);

	// Destroy textures
	TextureManagerDestroy();

	// Destroy Sound Buffer
	SoundBufferDestroy();

	// Destroy Audio Device
	AudioDestroyDevice();

	// Destroy the window handle
	WindowClose();

	// Destroy the protocol
	ProtocolDestroy();

	// Destroy cpu queries
	CPUDestroy();

	return true;
}

////////////////////////////////////////////////////////////
/// Set up the status of the game
////////////////////////////////////////////////////////////
void GameSetStatus(UInt32 StatusController)
{
	switch (StatusController)
	{
		case GAME_INITIALIZE :

			break;

		case GAME_LOGIN :
			LoginOpen();

			break;

		case GAME_ACCOUNT :
			// Close the login
			LoginClose();

			UserGetData()->Logged = true;

			break;

		case GAME_RUN :
			// Terrain test
			LoginClose();

			break;

		case GAME_DESTROY :

			break;
	}

	// Set the status
    MutexLock(&GameStatusMutex);

	GameStatus = StatusController;

	MutexUnlock(&GameStatusMutex);
}

////////////////////////////////////////////////////////////
/// Get the status of the game
////////////////////////////////////////////////////////////
void GameGetStatus(UInt32 * StatusController)
{
    MutexLock(&GameStatusMutex);

    *StatusController = GameStatus;

    MutexUnlock(&GameStatusMutex);
}

