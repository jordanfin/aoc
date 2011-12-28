#ifndef	MATH_GEOMETRY_MATRIX_H
#define MATH_GEOMETRY_MATRIX_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Math/Geometry/Vector.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Matrix3x3f
{
	float m[3][3];
};

struct Matrix4x4f
{
	float m[4][4];
};

float Matrix3x3fDeterminant(struct Matrix3x3f * Matrix);

void Matrix4x4fMultiply(struct Matrix4x4f * Matrix, struct Matrix4x4f * A, struct Matrix4x4f * B);
void Matrix4x4fVertor3fMultiply(struct Vector3f * VertOut, struct Matrix4x4f * Matrix, struct Vector3f * Vert);
void Matrix4x4fToGlMatrix(struct Matrix4x4f * Matrix, GLfloat * GLMatrix);
void Matrix4x4fFromGlMatrix(struct Matrix4x4f * Matrix, GLfloat * GLMatrix);

#endif // MATH_GEOMETRY_MATRIX_H