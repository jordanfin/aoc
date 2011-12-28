#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Char
{
	UInt16 Body, Head, Helmet, Weapon, Shield;	///< Appearance

};

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Character create
////////////////////////////////////////////////////////////
struct Char * CharacterCreate();

////////////////////////////////////////////////////////////
/// Character initialize
////////////////////////////////////////////////////////////
void CharacterInitialize(UInt16 Body, UInt16 Head, UInt16 Helmet, UInt16 Weapon, UInt16 Shield);

////////////////////////////////////////////////////////////
/// Character erase
////////////////////////////////////////////////////////////
void CharacterErase(struct Char * Character);

////////////////////////////////////////////////////////////
/// Character destroy
////////////////////////////////////////////////////////////
void CharacterDestroy(struct Char * Character);

#endif // GAME_CHARACTER_H