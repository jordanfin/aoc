////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Math/Geometry/Vector.h>

void Vector2fCopy(struct Vector2f * Dest, struct Vector2f * Src)
{
	if (Dest && Src)
	{
		Dest->x = Src->x; Dest->y = Src->y;
	}
}

void Vector2fSet(struct Vector2f * Vector, float x, float y)
{
	if (Vector)
	{
		Vector->x = x;
		Vector->y = y;
	}
}

void Vector3fCopy(struct Vector3f * Dest, struct Vector3f * Src)
{
	if (Dest && Src)
	{
		Dest->x = Src->x; Dest->y = Src->y; Dest->z = Src->z;
	}
}

void Vector3fSetNull(struct Vector3f * Vector)
{
	if (Vector)
	{
		Vector->x = Vector->y = Vector->z = 0.0f;
	}
}

void Vector3fSet(struct Vector3f * Vector, float x, float y, float z)
{
	if (Vector)
	{
		Vector->x = x;
		Vector->y = y;
		Vector->z = z;
	}
}

void Vector3fAdd(struct Vector3f * Dest, struct Vector3f * Src)
{
	if (Dest && Src)
	{
		Dest->x += Src->x;
		Dest->y += Src->y;
		Dest->z += Src->z;
	}
}

void Vector3fSubtract(struct Vector3f * Dest, struct Vector3f * Src)
{
	if (Dest && Src)
	{
		Dest->x -= Src->x;
		Dest->y -= Src->y;
		Dest->z -= Src->z;
	}
}

void Vector3fIncrement(struct Vector3f * Vector, float x, float y, float z)
{
	if (Vector)
	{
		Vector->x += x;
		Vector->y += y;
		Vector->z += z;
	}
}

void Vector3fDecrement(struct Vector3f * Vector, float x, float y, float z)
{
	if (Vector)
	{
		Vector->x -= x;
		Vector->y -= y;
		Vector->z -= z;
	}
}

void Vector3fMultiply(struct Vector3f * Vector, float Scale)
{
	if (Vector)
	{
		Vector->x *= Scale;
		Vector->y *= Scale;
		Vector->z *= Scale;
	}
}

void Vector3fDivide(struct Vector3f * Vector, float Scale)
{
	if (Vector)
	{
		Vector->x /= Scale;
		Vector->y /= Scale;
		Vector->z /= Scale;
	}
}

void Vector3fOpposite(struct Vector3f * Dest, struct Vector3f * Src)
{
	if (Dest && Src)
	{
		Dest->x = -Src->x;
		Dest->y = -Src->y;
		Dest->z = -Src->z;
	}
}

float Vector3fLength(struct Vector3f * Vector)
{
	if (Vector)
	{
		return (float)sqrt(Vector->x * Vector->x + Vector->y * Vector->y + Vector->z * Vector->z);
	}

	return 0.0f;
}

float Vector3fLengthSquared(struct Vector3f * Vector)
{
	if (Vector)
	{
		return Vector->x * Vector->x + Vector->y * Vector->y + Vector->z * Vector->z;
	}

	return 0.0f;
}

void Vector3fNormalize(struct Vector3f * Vector)
{
	if (Vector)
	{
		float Length = Vector3fLength(Vector);
		Vector3fDivide(Vector, Length);
	}
}

float Vector3fDot(struct Vector3f * v1, struct Vector3f * v2)
{
	if (v1 && v2)
	{
		return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
	}

	return 0.0f;
}

void Vector3fCross(struct Vector3f * Dest, struct Vector3f * Src)
{
	if (Dest && Src)
	{
		Dest->x = Dest->y * Src->z - Dest->z * Src->y;
		Dest->y = Dest->z * Src->x - Dest->x * Src->z;
		Dest->z = Dest->x * Src->y - Dest->y * Src->x;
	}
}

void Vector3fToArray(struct Vector3f * Vector, float * Array)
{
	if (Vector && Array)
	{
		Array[0] = Vector->x;
		Array[1] = Vector->y;
		Array[2] = Vector->z;
	}
}

void Vector3fToGlArray(struct Vector3f * Vector, GLfloat * Array)
{
	if (Vector && Array)
	{
		Array[0] = (GLfloat)Vector->x;
		Array[1] = (GLfloat)Vector->y;
		Array[2] = (GLfloat)Vector->z;
	}
}

float Vector3fDistance(struct Vector3f * v1, struct Vector3f * v2)
{
	float x = (float)fabs(v1->x - v2->x);
	float y = (float)fabs(v1->y - v2->y);
	float z = (float)fabs(v1->z - v2->z);

	return (float)sqrt(x * x + y * y + z * z);
}

void glVector4fSet(struct glVector4f * Vector, float x, float y, float z, float w)
{
	if (Vector)
	{
		Vector->x = (GLfloat)x;
		Vector->y = (GLfloat)y;
		Vector->z = (GLfloat)z;
		Vector->w = (GLfloat)w;
	}
}