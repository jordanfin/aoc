////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Game/Application.h>

int main(int argc, char *argv[])
{
	// Application initialize
	if (!GameInitialize())
		return GameDestroy();

	// Run the game
	GameRun();

	// Application destroy
    return GameDestroy();
}
