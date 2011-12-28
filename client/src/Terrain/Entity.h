#ifndef TERRAIN_ENTITY_H
#define TERRAIN_ENTITY_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Terrain/Block.h>

//struct Block
//{
//	struct BlockNode * First, * Last;
//	struct Vector3f Center;
//};

struct Entity
{
	struct BoundingBox * Bounds;			///< Bounds of the entity



};

#endif // TERRAIN_ENTITY_H