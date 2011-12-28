////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Graphics/TextureManager.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureLoader.h>
#include <Game/Index.h>
#include <System/Pack.h>
#include <System/Platform.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define TEXTURE_ERROR	-1	///< Texture error

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct TextureCache TextureControl[TEXTURE_CACHE_SIZE] = {{0}};		///< Texture cache array
struct Texture 		TextureList[TEXTURE_LOAD_SIZE] = {{0}};			///< Texture data array

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////
Int32 TextureSearchFree();	///< Search the free slot
Int32 TextureLastUsed();	///< Search the last used texture

////////////////////////////////////////////////////////////
/// Returns the texture path
////////////////////////////////////////////////////////////
char * TexturePath(Int32 Index)
{
	char * Path = (char*)(malloc(sizeof(char) * 255));

	sprintf(Path, "data/graphics/%d.png", Index);

	return Path;
}

////////////////////////////////////////////////////////////
/// Returns the texture loaded into memory
////////////////////////////////////////////////////////////
bool TextureLoad(Int32 Index, UInt8 * Data, bool StaticTexture, struct Rect * SubTexture)
{
	// Index in the array
	Int32 TextureIndex;

	// If it is loaded
	if (TextureControl[Index].ID > 0)
	{
		TextureControl[Index].LastUsed = SystemGetTime();
		return true;
	}

	// Search free slot
	TextureIndex = TextureSearchFree();

	if (TextureIndex == TEXTURE_ERROR)
	{
		TextureIndex = TextureLastUsed();

		if (TextureIndex == TEXTURE_ERROR)
			return false;

		// Delete texture
		TextureDelete(&TextureList[TextureIndex]);
	}

	// So.. load it
	if (Data == NULL)
	{
	    // Obtain the path
	    char * Path = TexturePath(IndexGetGrh(Index)->FileNumber);

		if (SubTexture)
		{
			// Load from file by sub rect
			TextureLoadFromFileEx(Path, &TextureList[TextureIndex], SubTexture);
		}
		else
		{
			// Load from file
			TextureLoadFromFile(Path, &TextureList[TextureIndex]);
		}

		// Delete it
		Delete(Path);
	}
	else
	{
		char * FilePack = "data/pack/graphics.pak";
		Int32 BufferSize;

		// Load buffer from pack
		if (!PackLoadData(FilePack, IndexGetGrh(Index)->FileNumber, &Data, &BufferSize))
			return false;

		// Load from memory
		TextureLoadFromMemory(Data, BufferSize, &TextureList[TextureIndex]);
	}

	if (!TextureList[TextureIndex].Pixels)
		return false;

	// Load texture into OGL memory
	if (TextureCreate(&TextureList[TextureIndex], false, false))
	{
		// Set data
		TextureControl[Index].ID = TextureIndex;
		TextureControl[Index].LastUsed = SystemGetTime();
		TextureControl[Index].Delete = StaticTexture;
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////
/// Returns the texture data
////////////////////////////////////////////////////////////
struct Texture * TextureGetData(Int32 Index)
{
	return &TextureList[TextureControl[Index].ID];
}

////////////////////////////////////////////////////////////
/// Render a texture
////////////////////////////////////////////////////////////
void TextureRender(Int32 TextureID, Int32 X, Int32 Y, struct Rect * SubRect, struct Color4ub * Color)
{
	// Load it, if it isn't loaded
	if (TextureLoad(TextureID, NULL, false, SubRect))
	{
		// Render it
		TextureRenderEx(TextureGetData(TextureID), X, Y, SubRect, Color);
	}
}

////////////////////////////////////////////////////////////
/// Delete all textures
////////////////////////////////////////////////////////////
void TextureManagerDestroy()
{
	UInt32 i;

	for (i = 1; i < TEXTURE_LOAD_SIZE; i++)
	{
		// If the texture is loaded
		if (TextureList[i].TextureID != 0)
		{
			TextureDelete(&TextureList[i]);
		}
	}
}

////////////////////////////////////////////////////////////
/// Search free slot
////////////////////////////////////////////////////////////
Int32 TextureSearchFree()
{
	UInt32 i;

	for (i = 1; i < TEXTURE_LOAD_SIZE; i++)
	{
		if (TextureList[i].TextureID == 0)
			return i;
	}

	return TEXTURE_ERROR;
}

////////////////////////////////////////////////////////////
/// Search the last used texture
////////////////////////////////////////////////////////////
Int32 TextureLastUsed()
{
	UInt32 MinorTime = 0;
	UInt32 Dimension = 0;
	UInt32 i;

	for (i = 1; i < TEXTURE_CACHE_SIZE; i++)
	{
		if (TextureControl[i].Delete == false)
		{
			if (TextureControl[i].LastUsed < MinorTime)
			{
				MinorTime = TextureControl[i].LastUsed;
				Dimension = i;
			}
		}
	}

	if (MinorTime != 0)
		return TextureControl[Dimension].ID;
	else
		return TEXTURE_ERROR;
}
