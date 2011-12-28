////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Terrain/Frustum.h>

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Checks if a point is in the viewing volume
////////////////////////////////////////////////////////////
bool FrustumCheckPoint(struct FrustumType * Frustum, struct Vector3f * Point)
{
	UInt32 i;

	// Check if the point is inside all six planes of the view frustum
	for (i = 0; i < FRUSTUM_PLANES_SIZE; i++) 
	{
		// if (Frustum->Planes[i].a * Point->x + Frustum->Planes[i].b * Point->y + Frustum->Planes[i].c * Point->z <= 0.0f)
		if (PlaneDotCoord(&Frustum->Planes[i], Point) < 0.0f)
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////
/// Checks if a sphere is in the viewing volume
////////////////////////////////////////////////////////////
bool FrustumCheckSphere(struct FrustumType * Frustum, struct Vector3f * Position, float Radius)
{
	UInt32 i;

	// Check if the radius of the sphere is inside the view frustum
	for (i = 0; i < FRUSTUM_PLANES_SIZE; i++) 
	{
		if (PlaneDotCoord(&Frustum->Planes[i], Position) < -Radius)
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////
/// Checks if a sphere is in the viewing volume
////////////////////////////////////////////////////////////
UInt32 FrustumCheckSpherePosition(struct FrustumType * Frustum, struct Vector3f * Position, float Radius)
{
	UInt32 i, Count;
	float Distance;

	// Check if the radius of the sphere is inside the view frustum
	for (i = 0, Count = 0; i < FRUSTUM_PLANES_SIZE; i++) 
	{
		Distance = PlaneDotCoord(&Frustum->Planes[i], Position);

		if (Distance <= -Radius)
			return FRUSTUM_OUTSIDE;

		if (Distance > Radius)
			Count++;
	}

	return (Count == FRUSTUM_PLANES_SIZE) ? FRUSTUM_INSIDE : FRUSTUM_OVERLAP;
}

////////////////////////////////////////////////////////////
/// Checks if a sphere is in the viewing volume, return
/// the distance to the frustum on success
////////////////////////////////////////////////////////////
float FrustumCheckSphereDistance(struct FrustumType * Frustum, struct Vector3f * Position, float Radius)
{
	UInt32 i;
	float Distance;

	// Check if the radius of the sphere is inside the view frustum
	for (i = 0; i < FRUSTUM_PLANES_SIZE; i++) 
	{
		Distance = PlaneDotCoord(&Frustum->Planes[i], Position);
		if (Distance < -Radius)
			return 0;
	}

	return Distance + Radius;
}

bool FrustumCheckCube(struct FrustumType * Frustum, struct Vector3f * Position, float Radius)
{
	UInt32 i;

	// Check if any one point of the cube is in the view frustum
	for (i = 0; i < FRUSTUM_PLANES_SIZE; i++) 
	{
		struct Vector3f VectorAux;

		Vector3fSet(&VectorAux, Position->x - Radius, Position->y - Radius, Position->z - Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x + Radius, Position->y - Radius, Position->z - Radius);
		
		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x - Radius, Position->y + Radius, Position->z - Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x + Radius, Position->y + Radius, Position->z - Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x - Radius, Position->y - Radius, Position->z + Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x + Radius, Position->y - Radius, Position->z + Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x - Radius, Position->y + Radius, Position->z + Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x + Radius, Position->y + Radius, Position->z + Radius);
		
		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		return false;
	}

	return true;
}

UInt32 FrustumCheckCubePosition(struct FrustumType * Frustum, struct Vector3f * Position, float Radius)
{
	UInt32 i, j, k;

	// Check if any one point of the cube is in the view frustum
	for (i = 0, j = 0; i < FRUSTUM_PLANES_SIZE; i++) 
	{
		struct Vector3f VectorAux;
		
		k = 0;

		Vector3fSet(&VectorAux, Position->x - Radius, Position->y - Radius, Position->z - Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			k++;

		Vector3fSet(&VectorAux, Position->x + Radius, Position->y - Radius, Position->z - Radius);
		
		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			k++;

		Vector3fSet(&VectorAux, Position->x - Radius, Position->y + Radius, Position->z - Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			k++;

		Vector3fSet(&VectorAux, Position->x + Radius, Position->y + Radius, Position->z - Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			k++;

		Vector3fSet(&VectorAux, Position->x - Radius, Position->y - Radius, Position->z + Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			k++;

		Vector3fSet(&VectorAux, Position->x + Radius, Position->y - Radius, Position->z + Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			k++;

		Vector3fSet(&VectorAux, Position->x - Radius, Position->y + Radius, Position->z + Radius);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			k++;

		Vector3fSet(&VectorAux, Position->x + Radius, Position->y + Radius, Position->z + Radius);
		
		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			k++;

		if (k == 0)
			return FRUSTUM_OUTSIDE;

		if (k == 8)
			j++;
	}

	return (j == FRUSTUM_PLANES_SIZE) ? FRUSTUM_INSIDE : FRUSTUM_OVERLAP;
}

bool FrustumCheckRectangle(struct FrustumType * Frustum, struct Vector3f * Position, struct Vector3f * Size)
{
	UInt32 i;

	// Check if any of the 6 planes of the rectangle are inside the view frustum
	for (i = 0; i < FRUSTUM_PLANES_SIZE; i++)
	{
		struct Vector3f VectorAux;

		Vector3fSet(&VectorAux, Position->x - Size->x, Position->y - Size->y, Position->z - Size->z);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x + Size->x, Position->y - Size->y, Position->z - Size->z);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x - Size->x, Position->y + Size->y, Position->z - Size->z);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x - Size->x, Position->y - Size->y, Position->z + Size->z);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x + Size->x, Position->y + Size->y, Position->z - Size->z);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x + Size->x, Position->y - Size->y, Position->z + Size->z);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x - Size->x, Position->y + Size->y, Position->z + Size->z);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		Vector3fSet(&VectorAux, Position->x + Size->x, Position->y + Size->y, Position->z + Size->z);

		if (PlaneDotCoord(&Frustum->Planes[i], &VectorAux) >= 0.0f)
			continue;

		return false;
	}

	return true;
}

bool FrustumCheckPolygon(struct FrustumType * Frustum, struct Vector3f * Points, UInt32 PointSize)
{
	UInt32 i, j;

	// Check if the polygon is inside the view frustum
	for (i = 0; i < FRUSTUM_PLANES_SIZE; i++) 
	{
		for (j = 0; j < PointSize; j++)
		{
			if (PlaneDotCoord(&Frustum->Planes[i], &Points[j]) > 0)
				break;
		}
		
		if (j == PointSize)
			return false;
	}

	return true;
}

void FrustumUpdate(struct FrustumType * Frustum, float Depth)
{
	float MinZ, Result;
	GLfloat Projection[16];
    GLfloat ModelView[16];
	GLfloat ViewProjection[16];

	// Get the Projection matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelView);

	// Set the projection matrix mode
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

		// Get the projection matrix to update the z distance
		glGetFloatv(GL_PROJECTION_MATRIX, Projection);

		// Calculate the minimum z distance in the frustum
		MinZ = -Projection[14] / Projection[10];
		Result = Depth / (Depth - MinZ);
		Projection[10] = Result;
		Projection[14] = -Result * MinZ;

		// Update the projection matrix
		glLoadMatrixf(Projection);

		// Multiply both to obtain the view projection matrix
		glMultMatrixf(ModelView);

		// Get the view projection matrix
		glGetFloatv(GL_PROJECTION_MATRIX, ViewProjection);
	glPopMatrix();

	// Calculate right plane of frustum
	Frustum->Planes[FRUSTUM_RIGHT].a = ViewProjection[ 3] - ViewProjection[ 0]; 
	Frustum->Planes[FRUSTUM_RIGHT].b = ViewProjection[ 7] - ViewProjection[ 4];
	Frustum->Planes[FRUSTUM_RIGHT].c = ViewProjection[11] - ViewProjection[ 8];
	Frustum->Planes[FRUSTUM_RIGHT].d = ViewProjection[15] - ViewProjection[12];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_RIGHT]);

	// Calculate the left plane of frustum
	Frustum->Planes[FRUSTUM_LEFT].a = ViewProjection[ 3] + ViewProjection[ 0]; 
	Frustum->Planes[FRUSTUM_LEFT].b = ViewProjection[ 7] + ViewProjection[ 4];
	Frustum->Planes[FRUSTUM_LEFT].c = ViewProjection[11] + ViewProjection[ 8];
	Frustum->Planes[FRUSTUM_LEFT].d = ViewProjection[15] + ViewProjection[12];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_LEFT]);

	// Calculate the bottom plane of frustum
	Frustum->Planes[FRUSTUM_BOTTOM].a = ViewProjection[ 3] + ViewProjection[ 1]; 
	Frustum->Planes[FRUSTUM_BOTTOM].b = ViewProjection[ 7] + ViewProjection[ 5];
	Frustum->Planes[FRUSTUM_BOTTOM].c = ViewProjection[11] + ViewProjection[ 9];
	Frustum->Planes[FRUSTUM_BOTTOM].d = ViewProjection[15] + ViewProjection[13];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_BOTTOM]);

	// Calculate the top plane of frustum
	Frustum->Planes[FRUSTUM_TOP].a = ViewProjection[ 3] - ViewProjection[ 1]; 
	Frustum->Planes[FRUSTUM_TOP].b = ViewProjection[ 7] - ViewProjection[ 5];
	Frustum->Planes[FRUSTUM_TOP].c = ViewProjection[11] - ViewProjection[ 9];
	Frustum->Planes[FRUSTUM_TOP].d = ViewProjection[15] - ViewProjection[13];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_TOP]);

	// Calculate the far plane of frustum
	Frustum->Planes[FRUSTUM_FAR].a = ViewProjection[ 3] - ViewProjection[ 2]; 
	Frustum->Planes[FRUSTUM_FAR].b = ViewProjection[ 7] - ViewProjection[ 6];
	Frustum->Planes[FRUSTUM_FAR].c = ViewProjection[11] - ViewProjection[10];
	Frustum->Planes[FRUSTUM_FAR].d = ViewProjection[15] - ViewProjection[14];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_FAR]);

	// Calculate the near plane of frustum
	Frustum->Planes[FRUSTUM_NEAR].a = ViewProjection[ 3] + ViewProjection[ 2]; 
	Frustum->Planes[FRUSTUM_NEAR].b = ViewProjection[ 7] + ViewProjection[ 6];
	Frustum->Planes[FRUSTUM_NEAR].c = ViewProjection[11] + ViewProjection[10];
	Frustum->Planes[FRUSTUM_NEAR].d = ViewProjection[15] + ViewProjection[14];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_NEAR]);
}

void FrustumUpdateFaster(struct FrustumType * Frustum, float Depth)
{
	float MinZ, Result;
	GLfloat Projection[16];
    GLfloat ModelView[16];
	GLfloat ViewProjection[16];

    // Get the current projection matrix from OpenGL
    glGetFloatv( GL_PROJECTION_MATRIX, Projection );

    // Get the current modelview matrix from OpenGL
    glGetFloatv( GL_MODELVIEW_MATRIX, ModelView );

	// Calculate the minimum z distance in the frustum
	MinZ = -Projection[14] / Projection[10];
	Result = Depth / (Depth - MinZ);
	Projection[10] = Result;
	Projection[14] = -Result * MinZ;

	// This function will only work if you do not rotate or translate your projection matrix
	ViewProjection[ 0] = ModelView[ 0] * Projection[ 0];
    ViewProjection[ 1] = ModelView[ 1] * Projection[ 5];
    ViewProjection[ 2] = ModelView[ 2] * Projection[10] + ModelView[ 3] * Projection[14];
    ViewProjection[ 3] = ModelView[ 2] * Projection[11];

    ViewProjection[ 4] = ModelView[ 4] * Projection[ 0];
    ViewProjection[ 5] = ModelView[ 5] * Projection[ 5];
    ViewProjection[ 6] = ModelView[ 6] * Projection[10] + ModelView[ 7] * Projection[14];
    ViewProjection[ 7] = ModelView[ 6] * Projection[11];

    ViewProjection[ 8] = ModelView[ 8] * Projection[ 0];
    ViewProjection[ 9] = ModelView[ 9] * Projection[ 5];
    ViewProjection[10] = ModelView[10] * Projection[10] + ModelView[11] * Projection[14];
    ViewProjection[11] = ModelView[10] * Projection[11];

    ViewProjection[12] = ModelView[12] * Projection[ 0];
    ViewProjection[13] = ModelView[13] * Projection[ 5];
    ViewProjection[14] = ModelView[14] * Projection[10] + ModelView[15] * Projection[14];
    ViewProjection[15] = ModelView[14] * Projection[11];

	// Calculate right plane of frustum
	Frustum->Planes[FRUSTUM_RIGHT].a = ViewProjection[ 3] - ViewProjection[ 0]; 
	Frustum->Planes[FRUSTUM_RIGHT].b = ViewProjection[ 7] - ViewProjection[ 4];
	Frustum->Planes[FRUSTUM_RIGHT].c = ViewProjection[11] - ViewProjection[ 8];
	Frustum->Planes[FRUSTUM_RIGHT].d = ViewProjection[15] - ViewProjection[12];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_RIGHT]);

	// Calculate the left plane of frustum
	Frustum->Planes[FRUSTUM_LEFT].a = ViewProjection[ 3] + ViewProjection[ 0]; 
	Frustum->Planes[FRUSTUM_LEFT].b = ViewProjection[ 7] + ViewProjection[ 4];
	Frustum->Planes[FRUSTUM_LEFT].c = ViewProjection[11] + ViewProjection[ 8];
	Frustum->Planes[FRUSTUM_LEFT].d = ViewProjection[15] + ViewProjection[12];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_LEFT]);

	// Calculate the bottom plane of frustum
	Frustum->Planes[FRUSTUM_BOTTOM].a = ViewProjection[ 3] + ViewProjection[ 1]; 
	Frustum->Planes[FRUSTUM_BOTTOM].b = ViewProjection[ 7] + ViewProjection[ 5];
	Frustum->Planes[FRUSTUM_BOTTOM].c = ViewProjection[11] + ViewProjection[ 9];
	Frustum->Planes[FRUSTUM_BOTTOM].d = ViewProjection[15] + ViewProjection[13];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_BOTTOM]);

	// Calculate the top plane of frustum
	Frustum->Planes[FRUSTUM_TOP].a = ViewProjection[ 3] - ViewProjection[ 1]; 
	Frustum->Planes[FRUSTUM_TOP].b = ViewProjection[ 7] - ViewProjection[ 5];
	Frustum->Planes[FRUSTUM_TOP].c = ViewProjection[11] - ViewProjection[ 9];
	Frustum->Planes[FRUSTUM_TOP].d = ViewProjection[15] - ViewProjection[13];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_TOP]);

	// Calculate the far plane of frustum
	Frustum->Planes[FRUSTUM_FAR].a = ViewProjection[ 3] - ViewProjection[ 2]; 
	Frustum->Planes[FRUSTUM_FAR].b = ViewProjection[ 7] - ViewProjection[ 6];
	Frustum->Planes[FRUSTUM_FAR].c = ViewProjection[11] - ViewProjection[10];
	Frustum->Planes[FRUSTUM_FAR].d = ViewProjection[15] - ViewProjection[14];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_FAR]);

	// Calculate the near plane of frustum
	Frustum->Planes[FRUSTUM_NEAR].a = ViewProjection[ 3] + ViewProjection[ 2]; 
	Frustum->Planes[FRUSTUM_NEAR].b = ViewProjection[ 7] + ViewProjection[ 6];
	Frustum->Planes[FRUSTUM_NEAR].c = ViewProjection[11] + ViewProjection[10];
	Frustum->Planes[FRUSTUM_NEAR].d = ViewProjection[15] + ViewProjection[14];
	PlaneNormalize(&Frustum->Planes[FRUSTUM_NEAR]);
}

void FrustumRender(struct FrustumType * Frustum)
{
	// So, function to debug
	struct Vector3f Edges[8];

	PlaneIntersection(&Frustum->Planes[FRUSTUM_RIGHT], &Frustum->Planes[FRUSTUM_TOP],
					  &Frustum->Planes[FRUSTUM_FAR], &Edges[0]);
	PlaneIntersection(&Frustum->Planes[FRUSTUM_RIGHT], &Frustum->Planes[FRUSTUM_TOP],
					  &Frustum->Planes[FRUSTUM_NEAR], &Edges[1]);
	PlaneIntersection(&Frustum->Planes[FRUSTUM_RIGHT], &Frustum->Planes[FRUSTUM_BOTTOM],
					  &Frustum->Planes[FRUSTUM_NEAR], &Edges[2]);
	PlaneIntersection(&Frustum->Planes[FRUSTUM_RIGHT], &Frustum->Planes[FRUSTUM_BOTTOM],
					  &Frustum->Planes[FRUSTUM_FAR], &Edges[3]);

	PlaneIntersection(&Frustum->Planes[FRUSTUM_LEFT], &Frustum->Planes[FRUSTUM_TOP],
					  &Frustum->Planes[FRUSTUM_FAR], &Edges[4]);
	PlaneIntersection(&Frustum->Planes[FRUSTUM_LEFT], &Frustum->Planes[FRUSTUM_TOP],
					  &Frustum->Planes[FRUSTUM_NEAR], &Edges[5]);
	PlaneIntersection(&Frustum->Planes[FRUSTUM_LEFT], &Frustum->Planes[FRUSTUM_BOTTOM],
					  &Frustum->Planes[FRUSTUM_NEAR], &Edges[6]);
	PlaneIntersection(&Frustum->Planes[FRUSTUM_LEFT], &Frustum->Planes[FRUSTUM_BOTTOM],
					  &Frustum->Planes[FRUSTUM_FAR], &Edges[7]);

	glEnable(GL_BLEND);

	glColor4f(0.0f, 1.0f, 0.0f, 0.5f);

	glBegin(GL_QUADS);

		// Right face
		glVertex3f(Edges[0].x, Edges[0].y, Edges[0].z);
		glVertex3f(Edges[1].x, Edges[1].y, Edges[3].z);
		glVertex3f(Edges[2].x, Edges[2].y, Edges[2].z);
		glVertex3f(Edges[3].x, Edges[3].y, Edges[1].z);

		// Left face
		glVertex3f(Edges[4].x, Edges[4].y, Edges[4].z);
		glVertex3f(Edges[5].x, Edges[5].y, Edges[5].z);
		glVertex3f(Edges[6].x, Edges[6].y, Edges[6].z);
		glVertex3f(Edges[7].x, Edges[7].y, Edges[7].z);

		// Top face
		glVertex3f(Edges[0].x, Edges[0].y, Edges[0].z);
		glVertex3f(Edges[4].x, Edges[4].y, Edges[4].z);
		glVertex3f(Edges[5].x, Edges[5].y, Edges[5].z);
		glVertex3f(Edges[1].x, Edges[1].y, Edges[1].z);

		// Bottom face
		glVertex3f(Edges[2].x, Edges[2].y, Edges[2].z);
		glVertex3f(Edges[6].x, Edges[6].y, Edges[6].z);
		glVertex3f(Edges[7].x, Edges[7].y, Edges[7].z);
		glVertex3f(Edges[3].x, Edges[3].y, Edges[3].z);

		// Far face
		// ..

		// Near face
		// ..

	glEnd();

	glDisable(GL_BLEND);
}