////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Math/Rect.h>

////////////////////////////////////////////////////////////
/// Move the whole rectangle by the given offset
////////////////////////////////////////////////////////////
void RectMove(struct Rect * Rectangle, Int32 OffsetX, Int32 OffsetY)
{
    Rectangle->Left   += OffsetX;
    Rectangle->Right  += OffsetX;
    Rectangle->Top    += OffsetY;
    Rectangle->Bottom += OffsetY;
}

////////////////////////////////////////////////////////////
/// Check if a point is inside the rectangle's area
////////////////////////////////////////////////////////////
bool RectContains(struct Rect * Rectangle, Int32 X, Int32 Y)
{
    return (X >= Rectangle->Left) && (X <= Rectangle->Right) && (Y >= Rectangle->Top) && (Y <= Rectangle->Bottom);
}

////////////////////////////////////////////////////////////
/// Check intersection between two rectangles
////////////////////////////////////////////////////////////
bool RectIntersects(struct Rect * RectangleA, struct Rect * RectangleB, struct Rect * OverlappingRect)
{
    // Compute overlapping rect
    struct Rect Overlapping = { ((RectangleA->Left	> 	RectangleB->Left) 	? 	RectangleA->Left 	:	RectangleB->Left),
								((RectangleA->Top 	> 	RectangleB->Top) 	? 	RectangleA->Top 	: 	RectangleB->Top),
								((RectangleA->Right < 	RectangleB->Right) 	? 	RectangleA->Right	: 	RectangleB->Right),
								((RectangleA->Bottom < 	RectangleB->Bottom)	? 	RectangleA->Bottom	: 	RectangleB->Bottom) };

    // If overlapping rect is valid, then there is intersection
    if ((Overlapping.Left < Overlapping.Right) && (Overlapping.Top < Overlapping.Bottom))
    {
        if (OverlappingRect)
            *OverlappingRect = Overlapping;
        return true;
    }
    else
    {
        if (OverlappingRect)
        {
        	struct Rect NULLRect = { 0, 0, 0, 0 };
            *OverlappingRect = NULLRect;
        }
        return true;
    }
}
