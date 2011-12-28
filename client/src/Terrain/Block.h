#ifndef TERRAIN_BLOCK_H
#define TERRAIN_BLOCK_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Math/Geometry/Vector.h>

struct BlockNode
{
	struct Vector2f Position;
	struct BlockNode * Next;
};

struct Block
{
	struct BlockNode * First, * Last;
	struct Vector3f Center;
};

struct BlockNode * BlockCreateNode();
bool BlockAddNode(struct Block * Group, float x, float y);
void BlockRemoveNode(struct Block * Group, struct BlockNode * Node);
struct BlockNode * BlockFindNode(struct Block * Group, float x, float y);
void BlockDestroy(struct Block * Group);
bool BlockCheckCollisionPoint(struct Block * Group, float x, float y);



#endif // TERRAIN_BLOCK_H