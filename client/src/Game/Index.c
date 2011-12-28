////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Game/Index.h>
#include <Math/Geometry/Rect.h>
#include <Graphics/Color.h>
#include <System/Parser.h>
#include <System/IOHelper.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
/// Member data
////////////////////////////////////////////////////////////
// TODO: Improve the using of this list
struct GrhData  GrhList[0xFFFFF];		///< Grh list data  [1048575]
struct BodyData BodyList[0xFFFF];		///< Body list data [65353]
struct WalkData HeadList[0xFFFF];		///< Head list data
struct WalkData HelmetList[0xFFFF];		///< Helmet list data
struct WalkData WeaponList[0xFFFF];		///< Weapon list data
struct WalkData ShieldList[0xFFFF];		///< Shield list data
struct FxData	FxList[0xFFFF];			///< Fx list data
UInt8			MapRainList[0xFFFF];	///< Map rain list data
struct Color4ub	ColorList[0xFF];		///< Color list data

UInt32 GrhCounter, BodyCounter, HeadCounter, HelmetCounter,
	   WeaponCounter, ShieldCounter, ColorCounter, FxCounter, MapRainCounter;

////////////////////////////////////////////////////////////
/// Load head grhs (binary access)
////////////////////////////////////////////////////////////
bool IndexLoadHeads()
{
	FILE * File;
	UInt32 i;
	struct WalkLoadData HeadLoadList[0xFFFF];

	if (!fopen(&File, "data/init/heads.ind", "rb"))
		return false;

	// Seek Header
	fseek(File, 255 + (sizeof(Int32) * 2), SEEK_SET);

	HelmetCounter = 0;

	fread(&HelmetCounter, sizeof(Int16), 1, File);

	if (HelmetCounter <= 0)
		return false;

	for (i = 1; i <= HelmetCounter; i++)
	{
		fread(&HeadLoadList[i], sizeof(struct WalkLoadData), 1, File);

		if (HeadLoadList[i].Walk[0])
		{
			IndexInitializeGrh(&HelmetList[i].Walk[0], HeadLoadList[i].Walk[0], 0);
			IndexInitializeGrh(&HelmetList[i].Walk[1], HeadLoadList[i].Walk[1], 0);
			IndexInitializeGrh(&HelmetList[i].Walk[2], HeadLoadList[i].Walk[2], 0);
			IndexInitializeGrh(&HelmetList[i].Walk[3], HeadLoadList[i].Walk[3], 0);
		}
	}

	fclose(File);

	return true;
}

////////////////////////////////////////////////////////////
/// Load helmet grh (binary access)
////////////////////////////////////////////////////////////
bool IndexLoadHelmets()
{
	FILE * File;
	UInt32 i;
	struct WalkLoadData HeadLoadList[0xFFFF];

	if (!fopen(&File, "data/init/helmets.ind", "rb"))
		return false;

	// Seek Header
	fseek(File, 255 + (sizeof(Int32) * 2), SEEK_SET);

	HeadCounter = 0;

	fread(&HeadCounter, sizeof(Int16), 1, File);

	if (HeadCounter <= 0)
		return false;

	for (i = 1; i <= HeadCounter; i++)
	{
		fread(&HeadLoadList[i], sizeof(struct WalkLoadData), 1, File);

		if (HeadLoadList[i].Walk[0])
		{
			IndexInitializeGrh(&HeadList[i].Walk[0], HeadLoadList[i].Walk[0], 0);
			IndexInitializeGrh(&HeadList[i].Walk[1], HeadLoadList[i].Walk[1], 0);
			IndexInitializeGrh(&HeadList[i].Walk[2], HeadLoadList[i].Walk[2], 0);
			IndexInitializeGrh(&HeadList[i].Walk[3], HeadLoadList[i].Walk[3], 0);
		}
	}

	fclose(File);

	return true;
}

////////////////////////////////////////////////////////////
/// Load body data and grhs (binary access)
////////////////////////////////////////////////////////////
bool IndexLoadBodies()
{
	FILE * File;
	UInt32 i;
	struct BodyDataLoad BodyLoadList[0xFFFF];

	if (!fopen(&File, "data/init/bodies.ind", "rb"))
		return false;

	// Seek Header
	fseek(File, 255 + (sizeof(Int32) * 2), SEEK_SET);

	BodyCounter = 0;

	fread(&BodyCounter, sizeof(Int16), 1, File);

	if (BodyCounter <= 0)
		return false;

	for (i = 1; i <= BodyCounter; i++)
	{
		fread(&BodyLoadList[i], sizeof(struct BodyDataLoad), 1, File);

		if (BodyLoadList[i].Body[0])
		{
			IndexInitializeGrh(&BodyList[i].Walk[0], BodyLoadList[i].Body[0], 0);
			IndexInitializeGrh(&BodyList[i].Walk[1], BodyLoadList[i].Body[1], 0);
			IndexInitializeGrh(&BodyList[i].Walk[2], BodyLoadList[i].Body[2], 0);
			IndexInitializeGrh(&BodyList[i].Walk[3], BodyLoadList[i].Body[3], 0);

			BodyList[i].HeadOffsetX = BodyLoadList[i].HeadOffsetX;
			BodyList[i].HeadOffsetY = BodyLoadList[i].HeadOffsetY;
		}
	}

	fclose(File);

	return true;
}

////////////////////////////////////////////////////////////
/// Load Fxs data (binary access)
////////////////////////////////////////////////////////////
bool IndexLoadFxs()
{
	FILE * File;
	UInt32 i;

	if (!fopen(&File, "data/init/fxs.ind", "rb"))
		return false;

	// Seek Header
	fseek(File, 255 + (sizeof(Int32) * 2), SEEK_SET);

	FxCounter = 0;

	fread(&FxCounter, sizeof(Int16), 1, File);

	if (FxCounter <= 0)
		return false;

	for (i = 1; i <= FxCounter; i++)
		fread(&FxList[i], sizeof(struct FxData), 1, File);

	fclose(File);

	return true;
}

////////////////////////////////////////////////////////////
/// Load map rain array (binary access)
////////////////////////////////////////////////////////////
bool IndexLoadMapRain()
{
	FILE * File;
	UInt32 i;

	if (!fopen(&File, "data/init/fk.ind", "rb"))
		return false;

	// Seek Header
	fseek(File, 255 + (sizeof(Int32) * 2), SEEK_SET);

	MapRainCounter = 0;

	fread(&MapRainCounter, sizeof(Int16), 1, File);

	if (MapRainCounter <= 0)
		return false;

	for (i = 1; i <= MapRainCounter; i++)
	{
		fread(&MapRainList[i], sizeof(UInt8), 1, File);
	}

	fclose(File);

	return true;
}

////////////////////////////////////////////////////////////
/// Load all grh data (binary access)
////////////////////////////////////////////////////////////
bool IndexLoadGrh()
{
	FILE * File;
	UInt32 i, GrhIndex = 0; GrhCounter = 0;

	if (!fopen(&File, "data/init/graphics.ind", "rb"))
		return false;

	// Seek FileVersion
	fseek(File, sizeof(Int32), SEEK_SET);

	// Get number of grh
	fread(&GrhCounter, sizeof(Int32), 1, File);

	if (GrhCounter <= 0)
		goto ErrorHandler;

	while (GrhIndex < GrhCounter)
	{
		// Get grh index
		fread(&GrhIndex, sizeof(Int32), 1, File);

		// Begin grh
		fread(&GrhList[GrhIndex].FramesCounter, sizeof(Int16), 1, File);

		if (GrhList[GrhIndex].FramesCounter <= 0)
			goto ErrorHandler;

		if (GrhList[GrhIndex].FramesCounter > 1)
		{
			// In case of animation
			for (i = 1; i <= GrhList[GrhIndex].FramesCounter; i++)
			{
				fread(&GrhList[GrhIndex].Frames[i], sizeof(Int32), 1, File);

				if ( (GrhList[GrhIndex].Frames[i] <= 0) || (GrhList[GrhIndex].Frames[i] > GrhCounter) )
					goto ErrorHandler;
			}

			fread(&GrhList[GrhIndex].Speed, sizeof(float), 1, File);
			if (GrhList[GrhIndex].Speed <= 0)
				goto ErrorHandler;

			// Compute width and height
			GrhList[GrhIndex].PixelHeight = GrhList[GrhList[GrhIndex].Frames[1]].PixelHeight;
			if (GrhList[GrhIndex].PixelHeight <= 0)
				goto ErrorHandler;

			GrhList[GrhIndex].PixelWidth = GrhList[GrhList[GrhIndex].Frames[1]].PixelWidth;
			if (GrhList[GrhIndex].PixelWidth <= 0)
				goto ErrorHandler;

			GrhList[GrhIndex].TileHeight = GrhList[GrhList[GrhIndex].Frames[1]].TileHeight;
			if (GrhList[GrhIndex].TileHeight <= 0)
				goto ErrorHandler;

			GrhList[GrhIndex].TileWidth = GrhList[GrhList[GrhIndex].Frames[1]].TileWidth;
			if (GrhList[GrhIndex].TileWidth <= 0)
				goto ErrorHandler;

			GrhList[GrhIndex].FileNumber = 0;
			GrhList[GrhIndex].sX		 = 0;
			GrhList[GrhIndex].sY		 = 0;
		}
		else
		{
			// In case of normal grh data
			fread(&GrhList[GrhIndex].FileNumber, sizeof(Int32), 1, File);
			if (GrhList[GrhIndex].FileNumber <= 0)
				goto ErrorHandler;

			fread(&GrhList[GrhIndex].sX, sizeof(Int16), 1, File);
			if (GrhList[GrhIndex].sX < 0)
				goto ErrorHandler;

			fread(&GrhList[GrhIndex].sY, sizeof(Int16), 1, File);
			if (GrhList[GrhIndex].sY < 0)
				goto ErrorHandler;

			fread(&GrhList[GrhIndex].PixelWidth, sizeof(Int16), 1, File);
			if (GrhList[GrhIndex].PixelWidth <= 0)
				goto ErrorHandler;

			fread(&GrhList[GrhIndex].PixelHeight, sizeof(Int16), 1, File);
			if (GrhList[GrhIndex].PixelHeight <= 0)
				goto ErrorHandler;

			// Compute width and height
			GrhList[GrhIndex].TileWidth = (float)GrhList[GrhIndex].PixelWidth / 32;
			GrhList[GrhIndex].TileHeight = (float)GrhList[GrhIndex].PixelHeight / 32;

			GrhList[GrhIndex].Frames[1] = GrhIndex;
			GrhList[GrhIndex].Speed		= 0;
		}
	}

	fclose(File);

	return true;

ErrorHandler:
	fclose(File);
	return false;
}

////////////////////////////////////////////////////////////
/// Load shield animations (ini access)
////////////////////////////////////////////////////////////
bool IndexLoadShields()
{
	UInt32 i, j;
	char Section[32];

	struct Dictionary * Data = ParserLoad("data/init/shields.dat");

	if (Data == NULL)
		return false;

	ShieldCounter = ParserGetInt(Data, "INIT:NumEscudos", -1);

	for (i = 0; i < ShieldCounter; i++)
	{
		for (j = 0; j < 4; j++)
		{
			sprintf(Section, "ESC%d:Dir%d", (i+1), (j+1));
			IndexInitializeGrh(&ShieldList[i].Walk[j], ParserGetInt(Data, Section, -1), 0);
		}
	}

	ParserFreeDict(Data);

	return true;
}

////////////////////////////////////////////////////////////
/// Load weapon animations (ini access)
////////////////////////////////////////////////////////////
bool IndexLoadWeapons()
{
	UInt32 i, j;
	char Section[32];

	struct Dictionary * Data = ParserLoad("data/init/weapons.dat");

	if (Data == NULL)
		return false;

	WeaponCounter = ParserGetInt(Data, "INIT:NumArmas", -1);

	for (i = 0; i < ShieldCounter; i++)
	{
		for (j = 0; j < 4; j++)
		{
			sprintf(Section, "ARMA%d:Dir%d", (i+1), (j+1));
			IndexInitializeGrh(&WeaponList[i].Walk[j], ParserGetInt(Data, Section, -1), 0);
		}
	}

	ParserFreeDict(Data);

	return true;
}

////////////////////////////////////////////////////////////
/// Load colour data (ini access)
////////////////////////////////////////////////////////////
bool IndexLoadColors()
{
	UInt32 i;
	char Section[32];

	struct Dictionary * Data = ParserLoad("data/init/colors.dat");

	if (Data == NULL)
		return false;

	for (i = 0; i < 50; i++)
	{
		sprintf(Section, "%d:R", i);
		ColorList[i].r = ParserGetInt(Data, Section, -1);
		sprintf(Section, "%d:G", i);
		ColorList[i].g = ParserGetInt(Data, Section, -1);
		sprintf(Section, "%d:B", i);
		ColorList[i].b = ParserGetInt(Data, Section, -1);
		ColorList[i].a = 255;
	}

	ColorList[50].r = ParserGetInt(Data, "Cr:R", -1);
	ColorList[50].g = ParserGetInt(Data, "Cr:G", -1);
	ColorList[50].b = ParserGetInt(Data, "Cr:B", -1);
	ColorList[50].a = 255;

	ColorList[51].r = ParserGetInt(Data, "Ci:R", -1);
	ColorList[51].g = ParserGetInt(Data, "Ci:G", -1);
	ColorList[51].b = ParserGetInt(Data, "Ci:B", -1);
	ColorList[51].a = 255;

	ColorList[52].r = ParserGetInt(Data, "At:R", -1);
	ColorList[52].g = ParserGetInt(Data, "At:G", -1);
	ColorList[52].b = ParserGetInt(Data, "At:B", -1);
	ColorList[52].a = 255;

	ParserFreeDict(Data);

	return true;
}

////////////////////////////////////////////////////////////
/// Initializer error helper
////////////////////////////////////////////////////////////
bool IndexShowError(char * Section)
{
	char Error[256];

	sprintf(Error, "An error ocurred when the client tried to load %s", Section);

	MessageError("IndexInitialize", Error);

	return false;
}

////////////////////////////////////////////////////////////
/// Initialize all data
////////////////////////////////////////////////////////////
bool IndexInitialize()
{
	if (!IndexLoadGrh())
		return IndexShowError("Grh Data");

	if (!IndexLoadBodies())
		return IndexShowError("Body Data");

	if (!IndexLoadHeads())
		return IndexShowError("Head Data");

	if (!IndexLoadHelmets())
		return IndexShowError("Helmet Data");

	if (!IndexLoadFxs())
		return IndexShowError("Fx Data");

	if (!IndexLoadMapRain())
		return IndexShowError("Map Rain Data");

	if (!IndexLoadWeapons())
		return IndexShowError("Weapon Data");

	if (!IndexLoadShields())
		return IndexShowError("Shield Data");

	if (!IndexLoadColors())
		return IndexShowError("Color Data");

	return true;
}

////////////////////////////////////////////////////////////
/// Sets up a grh to rendering
////////////////////////////////////////////////////////////
void IndexInitializeGrh(struct Grh *Data, Int32 Index, Int32 Started)
{
	Data->Index = Index;

	// By default
	if (Started == -1)
	{
		if (GrhList[Data->Index].FramesCounter > 1)
			Data->Started = 1;
		else
			Data->Started = 0;
	}
	else
	{
		// Make sure the graphic can be started
		if (GrhList[Data->Index].FramesCounter == 1)
			Started = 0;
		Data->Started = Started;
	}

	if (Data->Started)
		Data->Loops = -1;
	else
		Data->Loops = 0;

	Data->FrameCounter = 1;
	Data->Speed = GrhList[Data->Index].Speed;
}

////////////////////////////////////////////////////////////
/// Get size of the rect
////////////////////////////////////////////////////////////
void IndexGetSize(Int32 Index, Int32 * Width, Int32 * Height)
{
	*Width	= GrhList[Index].PixelWidth;
	*Height	= GrhList[Index].PixelHeight;
}

////////////////////////////////////////////////////////////
/// Get a grh from the list
////////////////////////////////////////////////////////////
struct GrhData * IndexGetGrh(UInt32 Index)
{
    if (Index > 0xFFFFF)
        return NULL;

	return &GrhList[Index];
}
