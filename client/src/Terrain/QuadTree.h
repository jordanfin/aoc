#ifndef TERRAIN_QUADTREE_H
#define TERRAIN_QUADTREE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
//#include <Terrain/Terrain.h>
//#include <Terrain/Frustum.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define QUAD_NODE_TYPE_NULL			0x00
#define QUAD_NODE_TYPE_ROOT			0x01
#define QUAD_NODE_TYPE_NODE			0x02
#define QUAD_NODE_TYPE_LEAF			0x03

#define QUADTREE_MAX_DEPTH			0x06
#define QUADTREE_MIN_ENTITIES		0x02
#define QUADTREE_MAX_ENTITIES		0x05

#define QUADTREE_NODE_TOP_LEFT		0x00
#define QUADTREE_NODE_TOP_RIGHT		0x01
#define QUADTREE_NODE_BOTTOM_LEFT	0x02
#define QUADTREE_NODE_BOTTOM_RIGHT	0x03

#define QUADTREE_NODE_SIZE			0x04

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct QuadNode
{
	struct BoundingBox * Bounds;				///< 2D bounds of the quad tree node
	struct QuadNode *	 Child[4], * Parent;	///< Children and parent nodes
	UInt32				 Type;					///< Quad tree node type
	UInt32				 Depth;					///< The depth of node in the tree
	struct Entity *		 EntityList;			///< List of entities
	UInt32				 EntitySize;			///< Size of the entities
};

struct QuadTreeType
{
	struct QuadNode * Root;						///< The root node of the quad tree

};

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Adds an entity to or removes one from the
/// children of this node
////////////////////////////////////////////////////////////
void QuadTreeUpdateEntity(struct QuadTreeType * QuadTree, struct EntityType * Entity, float x, float z, bool AddEntity)
{
	if (

	//UInt32 i, j;

	// Figure out in which child(ren) the entity belongs

	/*for (i = 0; i < QUADTREE_NODE_SIZE; i++)
	{
		if (i == 0)
			if (x - guy->radius() > centerX)
				continue;
		else if (x + guy->radius() < centerX)
			continue;
		
		for (int j = 0; j < 2; j++)
		{
			if (j == 0)
				if (z - guy->radius() > centerZ)
					continue;
			else if (z + guy->radius() < centerZ)
				continue;
			
			// Add or remove the entity
			if (AddEntity)
			{
				// children[i][j]->add(guy);
			}
			else
			{
				// children[i][j]->remove(guy, x, z);
			}
		}
	}*/
}

#endif // TERRAIN_QUADTREE_H