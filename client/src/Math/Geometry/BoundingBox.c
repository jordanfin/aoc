////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Math/Geometry/BoundingBox.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////
/// Create a bounding box
////////////////////////////////////////////////////////////
struct BoundingBox * BoundingBoxCreate()
{
	struct BoundingBox * Ptr = (struct BoundingBox*)malloc(sizeof(struct BoundingBox));

	Vector3fSetNull(&Ptr->Bounds[0]);
	Vector3fSetNull(&Ptr->Bounds[1]);
	Vector3fSetNull(&Ptr->Center);

	return Ptr;
}

////////////////////////////////////////////////////////////
/// Destroy a bounding box
////////////////////////////////////////////////////////////
void BoundingBoxDestroy(struct BoundingBox * Box)
{
	Delete(Box);
}

////////////////////////////////////////////////////////////
/// Set the data of a bounding box
////////////////////////////////////////////////////////////
void BoundingBoxSet(struct BoundingBox * Box, struct Vector3f * Min, struct Vector3f * Max)
{
	// Copy the vectors
	Vector3fCopy(&Box->Bounds[0], Min);
	Vector3fCopy(&Box->Bounds[1], Max);

	// Recalculate the center
	BoundingBoxUpdateCenter(Box);
}

////////////////////////////////////////////////////////////
/// Update the center of the bounding box
////////////////////////////////////////////////////////////
void BoundingBoxUpdateCenter(struct BoundingBox * Box)
{
	Vector3fSet(&Box->Center, Box->Bounds[0].x + (Box->Bounds[1].x - Box->Bounds[0].x) * 0.5f,
							  Box->Bounds[0].y + (Box->Bounds[1].y - Box->Bounds[0].y) * 0.5f,
							  Box->Bounds[0].z + (Box->Bounds[1].z - Box->Bounds[0].z) * 0.5f);
}

////////////////////////////////////////////////////////////
/// Create a bounding box from a list of vertices
////////////////////////////////////////////////////////////
void BoundingBoxGenerate(struct BoundingBox * Box, struct Vector3f * VertList, UInt32 Size)
{
	UInt32 i;

	// Get the minimum and maximum position
	for (i = 0; i < Size; i++)
	{
		// Check the minimum size
		if (Box->Bounds[0].x > VertList[i].x)
			Box->Bounds[0].x = VertList[i].x;

		if (Box->Bounds[0].y > VertList[i].y)
			Box->Bounds[0].y = VertList[i].y;

		if (Box->Bounds[0].z > VertList[i].z)
			Box->Bounds[0].z = VertList[i].z;

		// Check the maximum size
		if (Box->Bounds[1].x < VertList[i].x)
			Box->Bounds[1].x = VertList[i].x;

		if (Box->Bounds[1].y < VertList[i].y)
			Box->Bounds[1].y = VertList[i].y;

		if (Box->Bounds[1].z < VertList[i].z)
			Box->Bounds[1].z = VertList[i].z;

	}

	// Recalculate the center
	BoundingBoxUpdateCenter(Box);
}

////////////////////////////////////////////////////////////
/// Check if two bounding box are colliding
////////////////////////////////////////////////////////////
UInt32 BoundingBoxCollision(struct BoundingBox * Box, struct BoundingBox * Other)
{
	// Check if it is outside
	if (Box->Bounds[1].x < Other->Bounds[0].x || Box->Bounds[0].x > Other->Bounds[1].x ||
		Box->Bounds[1].y < Other->Bounds[0].y || Box->Bounds[0].y > Other->Bounds[1].y ||
		Box->Bounds[1].z < Other->Bounds[0].z || Box->Bounds[0].z > Other->Bounds[1].z)
		return BOUNDING_BOX_OUTSIDE;

	// Check if it is inside
	if (Box->Bounds[1].x <= Other->Bounds[1].x && Box->Bounds[0].x >= Other->Bounds[0].x &&
		Box->Bounds[1].y <= Other->Bounds[1].y && Box->Bounds[0].y >= Other->Bounds[0].y &&
		Box->Bounds[1].z <= Other->Bounds[1].z && Box->Bounds[0].z >= Other->Bounds[0].z)
		return BOUNDING_BOX_INSIDE;

	// If it aren't outside and inside, so overlap (collision)
	return BOUNDING_BOX_OVERLAP;
}

////////////////////////////////////////////////////////////
/// Check if a bounding box is colliding with a sphere (qri algorithm)
////////////////////////////////////////////////////////////
UInt32 BoundingBoxCollisionSphere(struct BoundingBox * Box, struct Vector3f * Sphere, float Radius)
{
	// TODO:
	//	http://www.mrtc.mdh.se/projects/3Dgraphics/paperF.pdf
	return 0;
}

////////////////////////////////////////////////////////////
/// Distance squared to a point from the box (arvos algorithm)
////////////////////////////////////////////////////////////
float BoundingBoxDistanceSquared(struct BoundingBox * Box, struct Vector3f * Point)
{
	struct Vector3f Distance = { 0.0f, 0.0f, 0.0f };


	return 0.0f;
}

////////////////////////////////////////////////////////////
/// Distance to a point from the box
////////////////////////////////////////////////////////////
float BoundingBoxDistance(struct BoundingBox * Box, struct Vector3f * Point)
{
	float DistanceSquared = BoundingBoxDistanceSquared(Box, Point);

	return (float)sqrt(DistanceSquared);
}

////////////////////////////////////////////////////////////
/// Longest axis of the bounding box
////////////////////////////////////////////////////////////
UInt32 BoundingBoxLongestAxis(struct BoundingBox * Box, float * Distance)
{
	UInt32 i, Dimension = 0;
	float Distances[3] = { (float)fabs(Box->Bounds[0].x - Box->Bounds[1].x),
						   (float)fabs(Box->Bounds[0].y - Box->Bounds[1].y),
						   (float)fabs(Box->Bounds[0].z - Box->Bounds[1].z) };

	float Temp = Distances[0];

    for (i = 1; i < 3; i++)
	{
		float Dist = max(Distances[i], Temp);
		
		if (Dist > Temp)
		{
			Temp = Dist;
			Dimension = i;
		}
    }

	if (Distance)
		*Distance = Temp;

	return Dimension;
}

////////////////////////////////////////////////////////////
/// Shortest axis of the bounding box
////////////////////////////////////////////////////////////
UInt32 BoundingBoxShortestAxis(struct BoundingBox * Box, float * Distance)
{
	UInt32 i, Dimension = 0;
	float Distances[3] = { (float)fabs(Box->Bounds[0].x - Box->Bounds[1].x),
						   (float)fabs(Box->Bounds[0].y - Box->Bounds[1].y),
						   (float)fabs(Box->Bounds[0].z - Box->Bounds[1].z) };
	float Temp = Distances[0];

    for (i = 1; i < 3; i++)
	{
		float Dist = min(Distances[i], Temp);
		
		if (Dist < Temp)
		{
			Temp = Dist;
			Dimension = i;
		}
    }

	if (Distance)
		*Distance = Temp;

	return Dimension;
}

////////////////////////////////////////////////////////////
/// Return the distance of an axis of the bounding box
////////////////////////////////////////////////////////////
float BoundingBoxAxisDistance(struct BoundingBox * Box, UInt32 Axis)
{
	if (Axis < 3)
	{
		float Distances[3] = { (float)fabs(Box->Bounds[0].x - Box->Bounds[1].x),
							   (float)fabs(Box->Bounds[0].y - Box->Bounds[1].y),
							   (float)fabs(Box->Bounds[0].z - Box->Bounds[1].z) };

		return Distances[Axis];
	}

	return 0.0f;
}