////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Terrain/TerrainLoader.h>

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Load a old map format
////////////////////////////////////////////////////////////
bool TerrainLoadMethod1(FILE * File)
{
	UInt8 Flags;
	Int16 Grh;
	UInt32 x, y;

	//if (!TerrainCreate(4, 1, 100, 100))
	//	return false;

	// Pass the header
	fseek(File, 273, SEEK_SET);

	// Read the map
	for (y = 0; y < 100; y++)
	{
		for (x = 0; x < 100; x++)
		{
			// Check the flags
			fread(&Flags, sizeof(UInt8), 1, File);
	//		TerrainGetTile(x, y)->Blocked = (Flags && 0x01);

			// Check the first layer
			fread(&Grh, sizeof(Int16), 1, File);
	//		IndexInitializeGrh(&TerrainGetTile(x, y)->Layers[0], Grh, -1);

			// Check the second layer
			if (Flags && 0x02)
			{
				fread(&Grh, sizeof(Int16), 1, File);
	//			IndexInitializeGrh(&TerrainGetTile(x, y)->Layers[1], Grh, -1);
			}
			else
			{
	//			TerrainGetTile(x, y)->Layers[1].Index = 0x00;
			}

			// Check the third layer
			if (Flags && 0x04)
			{
				fread(&Grh, sizeof(Int16), 1, File);
	//			IndexInitializeGrh(&TerrainGetTile(x, y)->Layers[2], Grh, -1);
			}
			else
			{
	//			TerrainGetTile(x, y)->Layers[2].Index = 0x00;
			}

			// Check the fourth layer
			if (Flags && 0x08)
			{
				fread(&Grh, sizeof(Int16), 1, File);
	//			IndexInitializeGrh(&TerrainGetTile(x, y)->Layers[3], Grh, -1);
			}
			else
			{
	//			TerrainGetTile(x, y)->Layers[3].Index = 0x00;
			}

			// Check the trigger
			if (Flags && 0x0A)
			{
	//			fread(&TerrainGetTile(x, y)->Trigger, sizeof(Int16), 1, File);
			}
			else
			{
	//			TerrainGetTile(x, y)->Trigger = 0x00;
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////
/// Load a old map format
////////////////////////////////////////////////////////////
bool TerrainLoadMethod2(FILE * File)
{
	UInt32 Width, Height, Layers;

	// Get the size of the map
	fread(&Width, sizeof(UInt32), 1, File);
	fread(&Height, sizeof(UInt32), 1, File);
	fread(&Layers, sizeof(UInt32), 1, File);

	//if (!TerrainCreate(Layers, 1, Width, Height))
	//	break;

	// Read all data
	// fread(TerrainGet(), sizeof(struct TerrainTile), Width * Height, File);

	return true;
}

////////////////////////////////////////////////////////////
/// Load the terrain from file
////////////////////////////////////////////////////////////
bool TerrainLoadFromFile(UInt8 Method, char * Path)
{
	FILE * File;
	bool Result = false;

	// Open it
	if (!fopen(&File, Path, "rb"))
		return false;

	switch (Method)
	{
		case TERRAIN_METHOD_1 :
		{
			Result = TerrainLoadMethod1(File);
			break;
		}

		case TERRAIN_METHOD_2 :
		{
			Result = TerrainLoadMethod2(File);
			break;
		}

		case TERRAIN_METHOD_3 :
		{

			break;
		}
	}

	// Close the file
	fclose(File);

	return Result;
}

////////////////////////////////////////////////////////////
/// Load the terrain from file
////////////////////////////////////////////////////////////
bool TerrainLoadFromMemory(UInt8 Method, UInt8 * Data, UInt32 Size)
{
	// Nothing yet
	return false;
}
