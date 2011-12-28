#ifndef TERRAIN_FRUSTUM_H
#define TERRAIN_FRUSTUM_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Math/Geometry/Plane.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define FRUSTUM_NEAR		0x00
#define FRUSTUM_FAR			0x01
#define FRUSTUM_LEFT		0x02
#define FRUSTUM_RIGHT		0x03
#define FRUSTUM_TOP			0x04
#define FRUSTUM_BOTTOM		0x05
#define FRUSTUM_PLANES_SIZE	0x06

#define FRUSTUM_OUTSIDE		0x00
#define FRUSTUM_INSIDE		0x01
#define FRUSTUM_OVERLAP		0x02

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct FrustumType
{
	struct Plane Planes[FRUSTUM_PLANES_SIZE];
};

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Checks if a point is in the viewing volume
////////////////////////////////////////////////////////////
bool FrustumCheckPoint(struct FrustumType * Frustum, struct Vector3f * Point);

////////////////////////////////////////////////////////////
/// Checks if a sphere is in the viewing volume
////////////////////////////////////////////////////////////
bool FrustumCheckSphere(struct FrustumType * Frustum, struct Vector3f * Position, float Radius);

////////////////////////////////////////////////////////////
/// Checks if a sphere is in the viewing volume
////////////////////////////////////////////////////////////
UInt32 FrustumCheckSpherePosition(struct FrustumType * Frustum, struct Vector3f * Position, float Radius);

////////////////////////////////////////////////////////////
/// Checks if a sphere is in the viewing volume, return
/// the distance to the frustum on success
////////////////////////////////////////////////////////////
float FrustumCheckSphereDistance(struct FrustumType * Frustum, struct Vector3f * Position, float Radius);

bool FrustumCheckCube(struct FrustumType * Frustum, struct Vector3f * Position, float Radius);

UInt32 FrustumCheckCubePosition(struct FrustumType * Frustum, struct Vector3f * Position, float Radius);

bool FrustumCheckRectangle(struct FrustumType * Frustum, struct Vector3f * Position, struct Vector3f * Size);

bool FrustumCheckPolygon(struct FrustumType * Frustum, struct Vector3f * Points, UInt32 PointSize);

void FrustumUpdate(struct FrustumType * Frustum, float Depth);

void FrustumUpdateFaster(struct FrustumType * Frustum, float Depth);

void FrustumRender(struct FrustumType * Frustum);

#endif // TERRAIN_FRUSTUM_H