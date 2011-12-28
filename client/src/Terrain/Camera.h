#ifndef TERRAIN_CAMERA_H
#define TERRAIN_CAMERA_H

////////////////////////////////////////////////////////////
// OpenGL 3D Camera implementation
//
// References:
//		Vic Hollis, NeHe articles (Lesson 44), July 2003
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Math/Geometry/Vector.h>
#include <Terrain/Frustum.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct CameraType
{
	// glVector vLightSourceToCamera, vLightSourceToIntersect;
	// glPoint m_LightSourcePos;
	
	// GLsizei m_WindowHeight;
	// GLsizei m_WindowWidth;

	// struct Vector3f ptIntersect, pt;

	float MaxPitchRate;
	float PitchDegrees;

	float MaxHeadingRate;
	float HeadingDegrees;
	
	float MaxForwardVelocity;
	float ForwardVelocity;
	
	struct Vector3f Position;
	struct Vector3f Direction;

	struct FrustumType Frustum;
};

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

void CameraInitialize(struct CameraType * Camera);
void CameraChangeVelocity(struct CameraType * Camera, float Velocity);
void CameraChangeHeading(struct CameraType * Camera, float Degrees);
void CameraChangePitch(struct CameraType * Camera, float Degrees);
void CameraChangePosition(struct CameraType * Camera, float x, float y, float z);
void CameraSetPrespective(struct CameraType * Camera);

#endif // TERRAIN_CAMERA_H