////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Math/Geometry/Plane.h>
#include <Math/Geometry/Matrix.h>

float PlaneNormalizeEx(struct Plane * Dest, struct Plane * Src)
{
	float Normal = PlaneGetNormal(Src);

	if (Normal)
	{
		Dest->a = Src->a / Normal;
		Dest->b = Src->b / Normal;
		Dest->c = Src->c / Normal;
		Dest->d = Src->d / Normal;
	}
	else
	{
		Dest->a = Dest->b = Dest->c = Dest->d = 0.0f;
	}

	return Normal;
}

float PlaneNormalize(struct Plane * P)
{
	float Normal = PlaneGetNormal(P);

	if (Normal)
	{
		P->a /= Normal;
		P->b /= Normal;
		P->c /= Normal;
		P->d /= Normal;
	}
	else
	{
		P->a = P->b = P->c = P->d = 0.0f;
	}

	return Normal;
}

float PlaneGetNormal(struct Plane * P)
{
	return (float)sqrt(P->a * P->a + P->b * P->b + P->c * P->c);
}

float PlaneDotCoord(struct Plane * P, struct Vector3f * Vector)
{
	if (!P || !Vector)
		return 0.0f;

	return P->a * Vector->x + P->b * Vector->y + P->c * Vector->z + P->d;
}

////////////////////////////////////////////////////////////
/// Return the intersection between three planes,
/// make sure that three planes are intersected in a point
////////////////////////////////////////////////////////////
float PlaneIntersection(struct Plane * p1, struct Plane * p2, struct Plane * p3, struct Vector3f * Point)
{
	struct Matrix3x3f A = { p1->a, p1->b, p1->c,
							p2->a, p2->b, p2->c,
							p3->a, p3->b, p3->c };

	struct Matrix3x3f Ax = { p1->d, p1->b, p1->c,
							 p2->d, p2->b, p2->c,
							 p3->d, p3->b, p3->c };

	struct Matrix3x3f Ay = { p1->a, p1->d, p1->c,
							 p2->a, p2->d, p2->c,
							 p3->a, p3->d, p3->c };

	struct Matrix3x3f Az = { p1->a, p1->b, p1->d,
							 p2->a, p2->b, p2->d,
							 p3->a, p3->b, p3->d };


	float DetA = Matrix3x3fDeterminant(&A);

	Point->x = Matrix3x3fDeterminant(&Ax) / DetA;
	Point->y = Matrix3x3fDeterminant(&Ay) / DetA;
	Point->z = Matrix3x3fDeterminant(&Az) / DetA;
}