////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Terrain/Terrain.h>
#include <Terrain/Camera.h>
#include <Terrain/Frustum.h>
#include <Graphics/OpenGLHelper.h>
#include <Graphics/TextureLoader.h>
#include <Graphics/Font.h>
#include <Window/Window.h>
#include <System/IOHelper.h>

#ifdef DEBUG
	char CameraText[0xFF] = {{0}};
#endif

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

struct TerrainType * TerrainCreate()
{
	struct TerrainType * Terrain = NULL;
	
	Terrain = (struct TerrainType*)malloc(sizeof(struct TerrainType));

	if (Terrain == NULL)
		return NULL;

	memset(Terrain, 0, sizeof(struct TerrainType));

	return Terrain;
}

bool TerrainInitialize(struct TerrainType * Terrain, UInt32 Method, UInt32 Width, UInt32 Height, char * Path)
{
	if (Terrain == NULL)
		return false;

	// Normals are not computed yet
	Terrain->ComputedNormals = false;

	// Create the camera
	Terrain->Camera = (struct CameraType*)malloc(sizeof(struct CameraType));

	if (!Terrain->Camera)
		return false;

	// Initialize the camera
	CameraInitialize(Terrain->Camera);

	// Set terrain stuff
	Terrain->Camera->MaxForwardVelocity = 50.0f;
	Terrain->Camera->MaxHeadingRate		= 360.0f;
	Terrain->Camera->MaxPitchRate		= 360.0f;
	Terrain->Camera->HeadingDegrees		= 0.0f;

	// Initialize the camera rotation and position
	CameraChangeHeading(Terrain->Camera, 180.0f);
	CameraChangePitch(Terrain->Camera, 40.0f);
	CameraChangePosition(Terrain->Camera, 0.0f, -10.0f, 0.0f);

	// Set terrain lights
	glVector4fSet(&Terrain->AmbientLight, 0.5f, 0.5f, 0.5f, 1.0f);
	glVector4fSet(&Terrain->LightColor, 0.5f, 0.5f, 0.5f, 1.0f);
	glVector4fSet(&Terrain->LightPos, -0.2f, 0.3f, -1.0f, 0.0f);

	// Create the height map
	if (!TerrainLoad(Terrain, Method, Width, Height, Path))
		return false;

	// Normalize the height of the height map
	TerrainHeightMapNormalize(Terrain);

	// Calculate the normals for the terrain data
	if (!TerrainCalculateNormals(Terrain))
		return false;

	// Calculate the texture coordinates
	TerrainCalculateTextureCoordinates(Terrain);

	// Initialize the vertex array that will hold the geometry for the terrain
	if (!TerrainGenerateBuffers(Terrain))
		return false;

	return true;
}

bool TerrainDestroy(struct TerrainType * Terrain)
{
	if (Terrain == NULL)
		return true;

	// Delete the camera
	Delete(Terrain->Camera);

	// Delete height map
	Delete(Terrain->HeightMap);

	// Delete the vertices
	Delete(Terrain->Vertices);

	// Delete texture atlas


	// Delete the terrain
	Delete(Terrain);

	return true;
}

bool TerrainLoad(struct TerrainType * Terrain, UInt32 Method, UInt32 Width, UInt32 Height, char * Path)
{
	switch (Method)
	{
		case TERRAIN_LOAD_DEFAULT :		///< Generate a defaul terrain
		{
			return TerrainHeightMapGenerate(Terrain, Width, Height);
		}

		case TERRAIN_LOAD_TEXTURE :		///< Load an heightmap from a texture
		{
			if (Path != NULL)
				return TerrainHeightMapLoadTexture(Terrain, Path);

			break;
		}

		case TERRAIN_LOAD_FILE :		///< Load the terrain from a file
		{
			if (Path != NULL)
				return TerrainHeightMapLoadFile(Terrain, Path);
			
			break;
		}
	}

	return false;
}

bool TerrainHeightMapGenerate(struct TerrainType * Terrain, UInt32 Width, UInt32 Height)
{
	UInt32 Index, i, j;

	// Get the terrain size
	Terrain->Width = Width;
	Terrain->Height = Height;

	// Create the structure to hold the height map data
	Terrain->HeightMap = (struct VertexType*)malloc(sizeof(struct VertexType) * Terrain->Width * Terrain->Height);

	if (!Terrain->HeightMap)
		return false;

	// Set the memory of the height map
	memset(Terrain->HeightMap, 0, sizeof(struct VertexType) * Terrain->Width * Terrain->Height);

	// Initialize the position in the Texture buffer
	for (j = 0; j < Terrain->Height; j++)
	{
		for (i = 0; i < Terrain->Width; i++)
		{			
			Index = (Terrain->Height * j) + i;

			Vector3fSet(&Terrain->HeightMap[Index].Position, (float)i, 0.0f, (float)j);
		}
	}

	return true;
}

bool TerrainHeightMapLoadFile(struct TerrainType * Terrain, char * Path)
{
	return false;
}

bool TerrainHeightMapLoadTexture(struct TerrainType * Terrain, char * Path)
{
	UInt32 Index, i, j;
	struct Texture HeightMapTex;

	// Load the texture
	if (!TextureLoadFromFile(Path, &HeightMapTex))
		return false;

	// Get the terrain size
	Terrain->Width = HeightMapTex.Width;
	Terrain->Height = HeightMapTex.Height;

	// Create the structure to hold the height map data
	Terrain->HeightMap = (struct VertexType*)malloc(sizeof(struct VertexType) * Terrain->Width * Terrain->Height);

	if (!Terrain->HeightMap)
	{
		// Delete memory
		Delete(HeightMapTex.Pixels);
		return false;
	}

	// Set the memory of the height map
	memset(Terrain->HeightMap, 0, sizeof(struct VertexType) * Terrain->Width * Terrain->Height);

	// Initialize the position in the Texture buffer
	for (j = 0; j < Terrain->Height; j++)
	{
		for (i = 0; i < Terrain->Width; i++)
		{			
			Index = (Terrain->Height * j) + i;

			// float h = height * (((float)HeightMapTex.Pixels[Index].r / 255.0f) - 0.5f); // height = parameter

			Vector3fSet(&Terrain->HeightMap[Index].Position, (float)i, (float)HeightMapTex.Pixels[Index].r, (float)j);
		}
	}
	
	// Delete memory
	Delete(HeightMapTex.Pixels);

	return true;
}

void TerrainHeightMapNormalize(struct TerrainType * Terrain)
{
	UInt32 i, j;

	for (j = 0; j < Terrain->Height; j++)
	{
		for (i = 0; i < Terrain->Width; i++)
		{
			Terrain->HeightMap[(Terrain->Height * j) + i].Position.y /= TERRAIN_NORMALIZATION_FACTOR;
		}
	}
}

bool TerrainCalculateNormals(struct TerrainType * Terrain)
{
	UInt32 i, j, Count;
	Int32 Index[4];
	struct Vector3f * Normals, Vertex[3], Vector[2], Summation;

	if (Terrain->ComputedNormals)
		return true;

	Normals = (struct Vector3f*)malloc(sizeof(struct Vector3f) * (Terrain->Height - 1) * (Terrain->Width - 1));

	// Create a temporary array to hold the un-normalized normal vectors
	if (!Normals)
		return false;

	// Go through all the faces in the mesh and calculate their normals
	for (j = 0; j < (Terrain->Height - 1); j++)
	{
		for (i = 0; i < (Terrain->Width - 1); i++)
		{
			Index[0] = (j * (Terrain->Height - 1)) + i;
			Index[1] = (j * Terrain->Height) + i;
			Index[2] = (j * Terrain->Height) + (i+1);
			Index[3] = ((j+1) * Terrain->Height) + i;

			Vector3fSetNull(&Vertex[0]);
			Vector3fSetNull(&Vertex[1]);
			Vector3fSetNull(&Vertex[2]);

			// Get three vertices from the face
			if (Index[1] > 0)
				Vector3fCopy(&Vertex[0], &Terrain->HeightMap[Index[1]].Position);

			if (Index[1] > 0)
				Vector3fCopy(&Vertex[1], &Terrain->HeightMap[Index[2]].Position);

			if (Index[1] > 0)
				Vector3fCopy(&Vertex[2], &Terrain->HeightMap[Index[3]].Position);

			// Calculate the two vectors for this face
			Vector3fSubtract(&Vertex[0], &Vertex[2]);
			Vector3fCopy(&Vector[0], &Vertex[0]);

			Vector3fSubtract(&Vertex[2], &Vertex[1]);
			Vector3fCopy(&Vector[1], &Vertex[2]);

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal
			Vector3fCross(&Vector[0], &Vector[1]);
			Vector3fCopy(&Normals[Index[0]], &Vector[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex
	for (j = 0; j < Terrain->Height; j++)
	{
		for (i = 0; i < Terrain->Width; i++)
		{
			// Initialize the summation and count
			Vector3fSetNull(&Summation);
			Count = 0;

			// Bottom left face
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				Index[0] = ((j - 1) * (Terrain->Height - 1)) + (i - 1);

				if (Index[0] > 0)
				{
					Vector3fAdd(&Summation, &Normals[Index[0]]);
					Count++;
				}
			}

			// Bottom right face
			if ((i < (Terrain->Width - 1)) && ((j - 1) >= 0))
			{
				Index[0] = ((j - 1) * (Terrain->Height - 1)) + i;

				if (Index[0] > 0)
				{
					Vector3fAdd(&Summation, &Normals[Index[0]]);
					Count++;
				}
			}

			// Upper left face
			if (((i - 1) >= 0) && (j < (Terrain->Height - 1)))
			{
				Index[0] = (j * (Terrain->Height - 1)) + (i - 1);

				if (Index[0] > 0)
				{
					Vector3fAdd(&Summation, &Normals[Index[0]]);
					Count++;
				}
			}

			// Upper right face
			if ((i < (Terrain->Width - 1)) && (j < (Terrain->Height - 1)))
			{
				Index[0] = (j * (Terrain->Height - 1)) + i;

				if (Index[0] > 0)
				{
					Vector3fAdd(&Summation, &Normals[Index[0]]);
					Count++;
				}
			}

			// Take the average of the faces touching this vertex
			Vector3fDivide(&Summation, (float)Count);
			
			// Get an index to the vertex location in the height map array
			Index[0] = (j * Terrain->Height) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array
			Vector3fNormalize(&Summation);
			Vector3fCopy(&Terrain->HeightMap[Index[0]].Normal, &Summation);
		}
	}

	// Release the temporary normals
	Delete(Normals);

	// Normals are computed
	Terrain->ComputedNormals = true;

	return true;
}

bool TerrainTextureLoad(struct TerrainType * Terrain, char * Path)
{
	return false;
}

void TerrainCalculateTextureCoordinates(struct TerrainType * Terrain)
{

}

bool TerrainGenerateBuffers(struct TerrainType * Terrain)
{
	UInt32 Index[5], i, j;
	float tu, tv;
	UInt32 Size = (Terrain->Height - 1) * (Terrain->Width - 1);

	// Calculate the number of vertices in the terrain mesh
	Terrain->VertexCount = (Terrain->Width - 1) * (Terrain->Height - 1) * 6;

	// Create the vertex array
	Terrain->Vertices = (struct VertexType*)malloc(sizeof(struct VertexType) * Terrain->VertexCount);

	if (!Terrain->Vertices)
		return false;

	// Initialize the index to the vertex buffer
	Index[0] = 0;

	// Load the vertex and index array with the terrain data
	for (j = 0; j < (Terrain->Height - 1); j++)
	{
		for (i = 0; i < (Terrain->Width - 1); i++)
		{
			Index[1] = (Terrain->Height * j) + i;				// Bottom left
			Index[2] = (Terrain->Height * j) + (i + 1);			// Bottom right
			Index[3] = (Terrain->Height * (j + 1)) + i;			// Upper left
			Index[4] = (Terrain->Height * (j + 1)) + (i + 1);	// Upper right

			//printf("%d - %f\n", Index[0],
			//		Vector3fDistance(&Terrain->Vertices[Index[0]].Position, &Terrain->Vertices[Index[1]].Position));

			// Upper left
			tv = Terrain->HeightMap[Index[3]].Texture.y;

			// Modify the texture coordinates to cover the top edge
			if (tv == 1.0f) { tv = 0.0f; }

			Vector3fCopy(&Terrain->Vertices[Index[0]].Position, &Terrain->HeightMap[Index[3]].Position);
			Vector2fSet(&Terrain->Vertices[Index[0]].Texture, Terrain->HeightMap[Index[3]].Texture.x, tv);
			Vector3fCopy(&Terrain->Vertices[Index[0]].Normal, &Terrain->HeightMap[Index[3]].Normal);
			Index[0]++;

			// Upper right
			tu = Terrain->HeightMap[Index[4]].Texture.x;
			tv = Terrain->HeightMap[Index[4]].Texture.y;

			// Modify the texture coordinates to cover the top and right edge
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			Vector3fCopy(&Terrain->Vertices[Index[0]].Position, &Terrain->HeightMap[Index[4]].Position);
			Vector2fSet(&Terrain->Vertices[Index[0]].Texture, tu, tv);
			Vector3fCopy(&Terrain->Vertices[Index[0]].Normal, &Terrain->HeightMap[Index[4]].Normal);
			Index[0]++;

			// Bottom left
			Vector3fCopy(&Terrain->Vertices[Index[0]].Position, &Terrain->HeightMap[Index[1]].Position);
			Vector2fCopy(&Terrain->Vertices[Index[0]].Texture, &Terrain->HeightMap[Index[1]].Texture);
			Vector3fCopy(&Terrain->Vertices[Index[0]].Normal, &Terrain->HeightMap[Index[1]].Normal);
			Index[0]++;

			// Bottom left
			Vector3fCopy(&Terrain->Vertices[Index[0]].Position, &Terrain->HeightMap[Index[1]].Position);
			Vector2fCopy(&Terrain->Vertices[Index[0]].Texture, &Terrain->HeightMap[Index[1]].Texture);
			Vector3fCopy(&Terrain->Vertices[Index[0]].Normal, &Terrain->HeightMap[Index[1]].Normal);
			Index[0]++;

			// Upper right
			tu = Terrain->HeightMap[Index[4]].Texture.x;
			tv = Terrain->HeightMap[Index[4]].Texture.y;

			// Modify the texture coordinates to cover the top and right edge
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			Vector3fCopy(&Terrain->Vertices[Index[0]].Position, &Terrain->HeightMap[Index[4]].Position);
			Vector2fSet(&Terrain->Vertices[Index[0]].Texture, tu, tv);
			Vector3fCopy(&Terrain->Vertices[Index[0]].Normal, &Terrain->HeightMap[Index[4]].Normal);
			Index[0]++;

			// Bottom right
			tu = Terrain->HeightMap[Index[2]].Texture.x;

			// Modify the texture coordinates to cover the right edge
			if (tu == 0.0f) { tu = 1.0f; }

			Vector3fCopy(&Terrain->Vertices[Index[0]].Position, &Terrain->HeightMap[Index[2]].Position);
			Vector2fSet(&Terrain->Vertices[Index[0]].Texture, tu, Terrain->HeightMap[Index[2]].Texture.y);
			Vector3fCopy(&Terrain->Vertices[Index[0]].Normal, &Terrain->HeightMap[Index[2]].Normal);
			Index[0]++;
		}
	}

	return true;
}

void TerrainSetHeight(struct TerrainType * Terrain, UInt32 x, UInt32 y, float Height)
{
	// Set the height
	Terrain->HeightMap[(Terrain->Height * y) + x].Position.y = Height;

	Terrain->ComputedNormals = false;
}

float TerrainGetHeight(struct TerrainType * Terrain, UInt32 x, UInt32 y)
{
	return Terrain->HeightMap[(Terrain->Height * y) + x].Position.y;
}

float TerrainGetHeightAt(struct TerrainType * Terrain, UInt32 x, UInt32 z)
{
	UInt32 LeftX, OutZ;
	float FracX, FracZ, Height[4];

	// Check if input are out of range
	MathCheckBounds((Int32*)&x, 0, (Terrain->Width - 1));
	MathCheckBounds((Int32*)&z, 0, (Terrain->Height - 1));
	
	// Compute the grid cell in which (x, z) lies and how close we are to the left and outward edges
	LeftX = x;

	if (LeftX == Terrain->Width - 1)
		LeftX--;

	FracX = (float)(x - LeftX);
	OutZ = z;

	if (OutZ == Terrain->Width - 1)
	{
		OutZ--;
	}

	FracZ = (float)(z - OutZ);
	
	// Compute the four heights for the grid cell
	Height[0] = TerrainGetHeight(Terrain, LeftX, OutZ);
	Height[1] = TerrainGetHeight(Terrain, LeftX, OutZ + 1);
	Height[2] = TerrainGetHeight(Terrain, LeftX + 1, OutZ);
	Height[3] = TerrainGetHeight(Terrain, LeftX + 1, OutZ + 1);
	
	// Take a weighted average of the four heights
	return (1 - FracX) * ((1 - FracZ) * Height[0] + FracZ * Height[1]) +
		FracX * ((1 - FracZ) * Height[2] + FracZ * Height[3]);
}

void TerrainUpdateCameraAngle(struct TerrainType * Terrain, struct Event * Evt, float Degrees)
{
	static struct Vector2f MousePosition = {{0}};
	static bool AngleUpdate = false;

	if (Evt->Type == MouseButtonPressed)
	{
		if (Evt->MouseButton.Button == RightButton)
			AngleUpdate = true;
	}

	if (Evt->Type == MouseButtonReleased)
	{
		if (Evt->MouseButton.Button == RightButton)
			AngleUpdate = false;
	}

	if (AngleUpdate)
	{
		if (Evt->Type == MouseMoved)
		{
			CameraChangeHeading(Terrain->Camera, -(MousePosition.x - Evt->MouseMove.X));
		}

		// Format string
		#ifdef DEBUG
		sprintf(CameraText, "Camera position (%.4f, %.4f, %.4f - Angle: %.4f)",
				-Terrain->Camera->Position.x, -Terrain->Camera->Position.y, -Terrain->Camera->Position.z,
				Terrain->Camera->HeadingDegrees);
		#endif
	}

	if (Evt->Type == MouseMoved)
	{
		MousePosition.x = (float)Evt->MouseMove.X;
		MousePosition.y = (float)Evt->MouseMove.Y;
	}
}

void TerrainUpdateMovement(struct TerrainType * Terrain, struct Event * Evt, float Distance)
{
	bool PositionUpdated = false;

	if (WindowIsKeyPressed(W))
	{
		float Cos = MathCos(degtorad(Terrain->Camera->HeadingDegrees + 90.0f));
		float Sin = MathSin(degtorad(Terrain->Camera->HeadingDegrees + 90.0f));

		CameraChangePosition(Terrain->Camera, Cos * Distance, 0.0f, Sin * Distance);
		PositionUpdated = true;
	}

	if (WindowIsKeyPressed(S))
	{
		float Cos = MathCos(degtorad(Terrain->Camera->HeadingDegrees + 90.0f));
		float Sin = MathSin(degtorad(Terrain->Camera->HeadingDegrees + 90.0f));
		
		CameraChangePosition(Terrain->Camera, Cos * -Distance, 0.0f, Sin * -Distance);
		PositionUpdated = true;
	}

	if (WindowIsKeyPressed(D))
	{
		float Cos = MathCos(degtorad(Terrain->Camera->HeadingDegrees));
		float Sin = MathSin(degtorad(Terrain->Camera->HeadingDegrees));

		CameraChangePosition(Terrain->Camera, Cos * -Distance, 0.0f, Sin * -Distance);
		PositionUpdated = true;
	}

	if (WindowIsKeyPressed(A))
	{
		float Cos = MathCos(degtorad(Terrain->Camera->HeadingDegrees));
		float Sin = MathSin(degtorad(Terrain->Camera->HeadingDegrees));

		CameraChangePosition(Terrain->Camera, Cos * Distance, 0.0f, Sin * Distance);
		PositionUpdated = true;
	}

	if (PositionUpdated)
	{
		float Height = TerrainGetHeightAt(Terrain, abs((UInt32)Terrain->Camera->Position.x), abs((UInt32)Terrain->Camera->Position.z));
		Terrain->Camera->Position.y = -10.0f - Height;

		// Format string
		#ifdef DEBUG
		sprintf(CameraText, "Camera position (%.4f, %.4f, %.4f - Angle: %.4f)",
				-Terrain->Camera->Position.x, -Terrain->Camera->Position.y, -Terrain->Camera->Position.z,
				Terrain->Camera->HeadingDegrees);
		#endif
	}
}

void TerrainRenderTest(struct TerrainType * Terrain)
{
	UInt32 i, j, k, Index;
	GLfloat Vector[3] = { 0.0f, 0.0f, 0.0f };

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );

	// Set the view matrix
	CameraSetPrespective(Terrain->Camera);

	// Construct the frustum
	FrustumUpdateFaster(&Terrain->Camera->Frustum, GL_HELPER_SCREEN_DEPTH);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (GLfloat*)&Terrain->AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, (GLfloat*)&Terrain->LightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat*)&Terrain->LightPos);

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glBegin(GL_TRIANGLES);
	
	for (j = 0, Index = 0; j < (Terrain->Height - 1); j++)
	{
		for (i = 0; i < (Terrain->Width - 1); i++)
		{
			for (k = 0; k < 6; k++)
			{
				//if (FrustumCheckPoint(&Terrain->Camera->Frustum, &Terrain->Vertices[Index].Position))
				//{
					Vector3fToGlArray(&Terrain->Vertices[Index].Normal, Vector);
					glNormal3fv(Vector);

					Vector3fToGlArray(&Terrain->Vertices[Index].Position, Vector);
					glVertex3fv(Vector);
				//}

				Index++;
			}
		}
	}

	glEnd();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glDisable( GL_LIGHT0 );
	glDisable( GL_NORMALIZE );
	glDisable( GL_COLOR_MATERIAL );
}