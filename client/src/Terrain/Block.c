
#include <Terrain/Block.h>

struct BlockNode * BlockCreateNode()
{
	return (struct BlockNode*)malloc(sizeof(struct BlockNode));
}

bool BlockAddNode(struct Block * Group, float x, float y)
{
	struct BlockNode * NewNode = BlockCreateNode();

	if (NewNode == NULL)
		return false;

	NewNode->Position.x = x;
	NewNode->Position.y = y;
	NewNode->Next = NULL;

	if (!Group->First)
	{
		Group->First = Group->Last = NewNode;
	}
	else
	{
		Group->Last->Next = NewNode;
		Group->Last = NewNode;
	}

	return true;
}

void BlockRemoveNode(struct Block * Group, struct BlockNode * Node)
{
	struct BlockNode * Iterator, * Previous = Group->First;

	for (Iterator = Group->First; Iterator != NULL; Iterator = Iterator->Next)
	{
		if (Iterator == Node)
		{
			// Remove it
			
			Previous->Next = Iterator->Next;
		}

		Previous = Iterator;
	}
}

struct BlockNode * BlockFindNode(struct Block * Group, float x, float y)
{
	struct BlockNode * Iterator;

	for (Iterator = Group->First; Iterator != NULL; Iterator = Iterator->Next)
	{
		if (Iterator->Position.x == x && Iterator->Position.y == y)
		{
			return Iterator;
		}
	}

	return NULL;
}

void BlockDestroy(struct Block * Group)
{
	struct BlockNode * Iterator = Group->First;

	while (Iterator != NULL)
	{
		struct BlockNode * Temp = Iterator->Next;

		// Delete it
		Delete(Iterator);

		Iterator = Temp;
	}

}

bool BlockCheckCollisionPoint(struct Block * Group, float x, float y)
{
	
	return true;
}
/*
////////////////////////////////////////////////////////////////////////////////
///
class DistancePointLineSegment
{
public:
    //****************************************************************************
    //* This constructor is the minimum distance function.
    //****************************************************************************
    DistancePointLineSegment( const Vector2& a,
                              const Vector2& b,
                              const Vector2& c )
    {
        // AC and AB are vectors with A as the origin.
        // Compute orthogonal projection of C onto AB as the scalar t.
        const Vector2 ac( c - a );
        const Vector2 ab( b - a );
        mT = DotProduct( ac, ab )
           / DotProduct( ab, ab );

        // If intersection is on the line SEGMENT (within A..B),
        // then t = {0,..,1}.
        if ( (mT >= 0.0f) and (mT <= 1.0f) )
        {
            mIfIntersectsLineSegment = true;
            mIntersection.x          = a.x + mT * (b.x - a.x);
            mIntersection.y          = a.y + mT * (b.y - a.y);
            mDistance                = Distance( mIntersection - c );
        }
        else
        {
            // The intersection is outside the line SEGMENT.
            // Instead, compute the distances between CA and CB
            // and return the minimum.
            mIfIntersectsLineSegment = false;
          //mIntersection.x          = N/A
          //mIntersection.y          = N/A
            mDistance                = std::min( Distance( c-a ), Distance( c-b ) );
        }
    }

public:
    bool        mIfIntersectsLineSegment;   ///< true if intersection on LINE SEGMENT (false if outside segment)
    fp          mT;                         ///< always valid
    Vector2     mIntersection;              ///< N/A if not mIfIntersectsLineSegment
    fp          mDistance;                  ///< minimum distance between point C and line segment AB
};
*/
