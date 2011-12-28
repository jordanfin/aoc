#ifndef	MATH_RECT_H
#define MATH_RECT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Rect
{
    Int32 Left;   ///< Left coordinate of the rectangle
    Int32 Top;    ///< Top coordinate of the rectangle
    Int32 Right;  ///< Right coordinate of the rectangle
    Int32 Bottom; ///< Bottom coordinate of the rectangle
};

////////////////////////////////////////////////////////////
/// Move the whole rectangle by the given offset
///
/// param Rectangle : Rect to move
/// param OffsetX : Horizontal offset
/// param OffsetY : Vertical offset
///
////////////////////////////////////////////////////////////
void RectMove(struct Rect * Rectangle, Int32 OffsetX, Int32 OffsetY);

////////////////////////////////////////////////////////////
/// Check if a point is inside the rectangle's area
///
/// param Rectangle : Rect to check
/// param X : X coordinate of the point to test
/// param Y : Y coordinate of the point to test
///
/// return true if the point is inside
///
////////////////////////////////////////////////////////////
bool RectContains(struct Rect * Rectangle, Int32 X, Int32 Y);

////////////////////////////////////////////////////////////
/// Check intersection between two rectangles
///
/// param Rectangle :       Rectangle to test
/// param OverlappingRect : Rectangle to be filled with overlapping rect (NULL by default)
///
/// return True if rectangles overlap
///
////////////////////////////////////////////////////////////
bool RectIntersects(struct Rect * RectangleA, struct Rect * RectangleB, struct Rect * OverlappingRect);

#endif // MATH_RECT_H
