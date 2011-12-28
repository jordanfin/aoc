////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Terrain/Camera.h>
#include <Graphics/OpenGLHelper.h>

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////
void CameraInitialize(struct CameraType * Camera)
{
	memset(Camera, 0, sizeof(struct CameraType));
}

void CameraChangeVelocity(struct CameraType * Camera, float Velocity)
{
	if (fabs(Velocity) < fabs(Camera->MaxForwardVelocity))
	{
		// Our velocity is less than the max velocity increment that we defined so lets increment it
		Camera->ForwardVelocity += Velocity;
	}
	else
	{
		// Our velocity is greater than the max velocity increment that we defined
		// so we can only increment our velocity by the maximum allowed value
		if (Velocity < 0)
		{
			// We are slowing down so decrement
			Camera->ForwardVelocity -= -Camera->MaxForwardVelocity;
		}
		else
		{
			// We are speeding up so increment
			Camera->ForwardVelocity += Camera->MaxForwardVelocity;
		}
	}
}

void CameraChangeHeading(struct CameraType * Camera, float Degrees)
{
	if(fabs(Degrees) < fabs(Camera->MaxHeadingRate))
	{
		// Our Heading is less than the max heading rate that we defined so lets increment it but
		// first we must check to see if we are inverted so that our heading will not become inverted
		if (Camera->PitchDegrees > 90.0f && Camera->PitchDegrees < 270.0f ||
		   (Camera->PitchDegrees < -90.0f && Camera->PitchDegrees > -270.0f))
		{
			Camera->HeadingDegrees -= Degrees;
		}
		else
		{
			Camera->HeadingDegrees += Degrees;
		}
	}
	else
	{
		// Our heading is greater than the max heading rate that we defined
		// so we can only increment our heading by the maximum allowed value
		if (Degrees < 0)
		{
			// Check to see if we are upside down.
			if ((Camera->PitchDegrees > 90.0f && Camera->PitchDegrees < 270.0f) ||
				(Camera->PitchDegrees < -90.0f && Camera->PitchDegrees > -270.0f))
			{
				// Ok we would normally decrement here but since we are upside down then we need to increment our heading
				Camera->HeadingDegrees += Camera->MaxHeadingRate;
			}
			else
			{
				// We are not upside down so decrement as usual
				Camera->HeadingDegrees -= Camera->MaxHeadingRate;
			}
		}
		else
		{
			// Check to see if we are upside down
			if (Camera->PitchDegrees > 90.0f && Camera->PitchDegrees < 270.0f ||
			   (Camera->PitchDegrees < -90.0f && Camera->PitchDegrees > -270.0f))
			{
				// Ok we would normally increment here but since we are upside down then we need to decrement our heading
				Camera->HeadingDegrees -= Camera->MaxHeadingRate;
			}
			else
			{
				// We are not upside down so increment as usual
				Camera->HeadingDegrees += Camera->MaxHeadingRate;
			}
		}
	}
	
	// We don't want our heading to run away from us either. Although it really doesn't
	// matter I prefer to have my heading degrees within the range of -360.0f to 360.0f
	if (Camera->HeadingDegrees > 360.0f)
	{
		Camera->HeadingDegrees -= 360.0f;
	}
	else if (Camera->HeadingDegrees < -360.0f)
	{
		Camera->HeadingDegrees += 360.0f;
	}
}

void CameraChangePitch(struct CameraType * Camera, float Degrees)
{
	if (fabs(Degrees) < fabs(Camera->MaxPitchRate))
	{
		// Our pitch is less than the max pitch rate that we defined so lets increment it
		Camera->PitchDegrees += Degrees;
	}
	else
	{
		// Our pitch is greater than the max pitch rate that we defined
		// so we can only increment our pitch by the maximum allowed value
		if (Degrees < 0)
		{
			// We are pitching down so decrement
			Camera->PitchDegrees -= Camera->MaxPitchRate;
		}
		else
		{
			// We are pitching up so increment
			Camera->PitchDegrees += Camera->MaxPitchRate;
		}
	}

	// We don't want our pitch to run away from us. Although it really doesn't
	// matter I prefer to have my pitch degrees within the range of -360.0f to 360.0f
	if (Camera->PitchDegrees > 360.0f)
	{
		Camera->PitchDegrees -= 360.0f;
	}
	else if (Camera->PitchDegrees < -360.0f)
	{
		Camera->PitchDegrees += 360.0f;
	}
}

void CameraChangePosition(struct CameraType * Camera, float x, float y, float z)
{
	Vector3fIncrement(&Camera->Position, x, y, z);
}

void CameraSetPrespective(struct CameraType * Camera)
{
	GLfloat Matrix[16];
	// struct Vector3f Vector;

	// Calculate direction vector
	glRotatef(Camera->HeadingDegrees, 0.0f, 1.0f, 0.0f);
	glRotatef(Camera->PitchDegrees, 1.0f, 0.0f, 0.0f);

	// Get the resulting matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, Matrix);

	// Get the direction vector from the matrix (inverted z axis)
	Camera->Direction.x = Matrix[8];
	Camera->Direction.y = Matrix[9];
	Camera->Direction.z = -Matrix[10];

	// Erase the results
	glLoadIdentity();

	// Rotate the scene to get the right orientation
	glRotatef(Camera->PitchDegrees, 1.0f, 0.0f, 0.0f);
	glRotatef(Camera->HeadingDegrees, 0.0f, 1.0f, 0.0f);

	// Scale the direction by our speed
	//Vector3fCopy(&Vector, &Camera->Direction);
	//Vector3fMultiply(&Vector, Camera->ForwardVelocity);

	// Increment our position by the vector
	//Vector3fAdd(&Camera->Position, &Vector);

	// Translate to the new position
	//glTranslatef(-Camera->Position.x, -Camera->Position.y, -Camera->Position.z);

	// Translate to the position
	glTranslatef(Camera->Position.x, Camera->Position.y, Camera->Position.z);
}