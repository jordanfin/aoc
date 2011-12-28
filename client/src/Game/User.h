#ifndef GAME_USER_H
#define GAME_USER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Game/Character.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Player
{
	char Name[0x20];
	UInt32 Gold;
	UInt16 Head, Body;
};

struct User
{
	char			Name[0x40];			///< User name
	char			Password[0x40];		///< User password
	bool			Logged;				///< User login state
	UInt32			PlayersSize;		///< User player size
	struct Player	Players[0x06];		///< User player list
	struct Char *	Character;			///< User char appearance
};

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// User initialize
////////////////////////////////////////////////////////////
void UserReset();

////////////////////////////////////////////////////////////
/// User destroy
////////////////////////////////////////////////////////////
void UserDestroy();

////////////////////////////////////////////////////////////
/// Returns user global data
////////////////////////////////////////////////////////////
struct User * UserGetData();

#endif // GAME_USER_H
