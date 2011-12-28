#ifndef GAME_INDEX_H
#define GAME_INDEX_H

////////////////////////////////////////////////////////////
/// Author comments:
///	This module manages all grh index, animations, and others.
///	The format it's a little bit HORRIBLE.. but i don't want
/// to make a new tools (like indexer), and new format.
/// It could be, in the near future.
/// Remember, we have one index more in all lists.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct GrhData					///< Contains info about grh, size and animation
{
	Int16 sX, sY;
	Int32 FileNumber;
	Int16 PixelWidth, PixelHeight;
	float TileWidth, TileHeight;
	UInt16 FramesCounter;
	UInt32 Frames[0xFF];
	float Speed;
};

struct Grh						///< Pointer to GrhData (animation)
{
	Int16 Index;
	float FrameCounter, Speed;
	bool  Started;
	Int16 Loops;
};

struct BodyData					///< List of bodies
{
	struct Grh Walk[4];
	Int32 HeadOffsetX, HeadOffsetY;
};

struct BodyDataLoad				///< List of bodies used for load from ind
{
	Int16 Body[4];
	Int16 HeadOffsetX, HeadOffsetY;
};

struct FxData					///< List of fx
{
	Int16 Animation, OffsetX, OffsetY;
};

///< Walk loader and data struct
struct WalkData { struct Grh Walk[4]; };
struct WalkLoadData { Int16 Walk[4]; };

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Initialize all data
////////////////////////////////////////////////////////////
bool IndexInitialize();

////////////////////////////////////////////////////////////
/// Sets up a grh to rendering
////////////////////////////////////////////////////////////
void IndexInitializeGrh(struct Grh * Data, Int32 Index, Int32 Started);

////////////////////////////////////////////////////////////
/// Get size of the rect
////////////////////////////////////////////////////////////
void IndexGetSize(Int32 Index, Int32 * Width, Int32 * Height);

////////////////////////////////////////////////////////////
/// Get a grh from the list
////////////////////////////////////////////////////////////
struct GrhData * IndexGetGrh(UInt32 Index);

#endif // GAME_INDEX_H
