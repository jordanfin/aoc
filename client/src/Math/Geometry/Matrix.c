////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Math/Geometry/Matrix.h>

float Matrix3x3fDeterminant(struct Matrix3x3f * Matrix)
{
	return (Matrix->m[0][0] * Matrix->m[1][1] * Matrix->m[2][2]) +
		   (Matrix->m[1][0] * Matrix->m[2][1] * Matrix->m[0][2]) +
		   (Matrix->m[0][1] * Matrix->m[1][2] * Matrix->m[2][0]) -
		   (Matrix->m[2][0] * Matrix->m[1][1] * Matrix->m[0][2]) -
		   (Matrix->m[1][0] * Matrix->m[0][1] * Matrix->m[2][2]) -
		   (Matrix->m[2][1] * Matrix->m[1][2] * Matrix->m[0][0]);
};

void Matrix4x4fMultiply(struct Matrix4x4f * Matrix, struct Matrix4x4f * A, struct Matrix4x4f * B)
{
	UInt32 i, j, k;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			for (k = 0, Matrix->m[i][j] = 0; k < 4; k++)
			{
				Matrix->m[i][j] += A->m[i][k] * B->m[k][j];
			}
		}
	}
}

void Matrix4x4fVertor3fMultiply(struct Vector3f * VertOut, struct Matrix4x4f * Matrix, struct Vector3f * Vert)
{
	VertOut->x = Vert->x * Matrix->m[0][0] + Vert->y * Matrix->m[0][1] + Vert->z * Matrix->m[0][2] + Matrix->m[0][3];
	VertOut->y = Vert->x * Matrix->m[1][0] + Vert->y * Matrix->m[1][1] + Vert->z * Matrix->m[1][2] + Matrix->m[1][3];
	VertOut->z = Vert->x * Matrix->m[2][0] + Vert->y * Matrix->m[2][1] + Vert->z * Matrix->m[2][2] + Matrix->m[2][3];
}

void Matrix4x4fToGlMatrix(struct Matrix4x4f * Matrix, GLfloat * GLMatrix)
{
	UInt32 i, j, k;

	for (i = 0, k = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			GLMatrix[k] = Matrix->m[i][j];
			k++;
		}
	}
}

void Matrix4x4fFromGlMatrix(struct Matrix4x4f * Matrix, GLfloat * GLMatrix)
{
	UInt32 i, j, k;

	for (i = 0, k = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			Matrix->m[i][j] = GLMatrix[k];
			k++;
		}
	}
}