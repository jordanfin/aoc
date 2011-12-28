#ifndef	MATH_GEOMETRY_PLANE_H
#define MATH_GEOMETRY_PLANE_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Math/Geometry/Vector.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Plane
{
	float a, b, c, d;
};

float PlaneNormalizeEx(struct Plane * Dest, struct Plane * Src);
float PlaneNormalize(struct Plane * P);
float PlaneGetNormal(struct Plane * P);
float PlaneDotCoord(struct Plane * P, struct Vector3f * Vector);
float PlaneDistance(struct Plane * P, struct Vector3f * Position);

////////////////////////////////////////////////////////////
/// Return the intersection between three planes,
/// make sure that three planes are intersected in a point
////////////////////////////////////////////////////////////
float PlaneIntersection(struct Plane * p1, struct Plane * p2, struct Plane * p3, struct Vector3f * Point);

#endif // MATH_GEOMETRY_PLANE_H