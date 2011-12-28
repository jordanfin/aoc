#ifndef	MATH_GEOMETRY_VECTOR_H
#define MATH_GEOMETRY_VECTOR_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Math/General.h>
#include <Graphics/OpenGLHelper.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Vector2f
{
	float x, y;
};

struct Vector3f
{
	float x, y, z;
};

struct glVector4f
{
	GLfloat x, y, z, w;
};

void Vector2fCopy(struct Vector2f * Dest, struct Vector2f * Src);

void Vector2fSet(struct Vector2f * Vector, float x, float y);

void Vector3fCopy(struct Vector3f * Dest, struct Vector3f * Src);

void Vector3fSetNull(struct Vector3f * Vector);

void Vector3fSet(struct Vector3f * Vector, float x, float y, float z);

void Vector3fAdd(struct Vector3f * Dest, struct Vector3f * Src);

void Vector3fSubtract(struct Vector3f * Dest, struct Vector3f * Src);

void Vector3fIncrement(struct Vector3f * Vector, float x, float y, float z);

void Vector3fDecrement(struct Vector3f * Vector, float x, float y, float z);

void Vector3fMultiply(struct Vector3f * Vector, float Scale);

void Vector3fDivide(struct Vector3f * Vector, float Scale);

void Vector3fOpposite(struct Vector3f * Dest, struct Vector3f * Src);

float Vector3fLength(struct Vector3f * Vector);

float Vector3fLengthSquared(struct Vector3f * Vector);

void Vector3fNormalize(struct Vector3f * Vector);

float Vector3fDot(struct Vector3f * v1, struct Vector3f * v2);

void Vector3fCross(struct Vector3f * Dest, struct Vector3f * Src);

void Vector3fToArray(struct Vector3f * Vector, float * Array);

void Vector3fToGlArray(struct Vector3f * Vector, GLfloat * Array);

float Vector3fDistance(struct Vector3f * v1, struct Vector3f * v2);

void glVector4fSet(struct glVector4f * Vector, float x, float y, float z, float w);

#endif // MATH_GEOMETRY_VECTOR_H
