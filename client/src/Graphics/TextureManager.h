#ifndef GRAPHICS_TEXTUREMANAGER_H
#define GRAPHICS_TEXTUREMANAGER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Graphics/Color.h>
#include <Math/Geometry/Rect.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define TEXTURE_CACHE_SIZE	0xFFFF	///< Texture control max number
#define TEXTURE_LOAD_SIZE	0xFF	///< Texture memory max size

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct TextureCache
{
	Int32 	ID;			///< Texture ID
	UInt32 	LastUsed;	///< Texture last used
	bool	Delete;		///< If the texture can be deleted or not
};

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Returns the texture loaded into memory
////////////////////////////////////////////////////////////
bool TextureLoad(Int32 Index, UInt8 * Data, bool StaticTexture, struct Rect * SubTexture);

////////////////////////////////////////////////////////////
/// Returns the texture data
////////////////////////////////////////////////////////////
struct Texture * TextureGetData(Int32 Index);

////////////////////////////////////////////////////////////
/// Render a texture
////////////////////////////////////////////////////////////
void TextureRender(Int32 TextureID, Int32 X, Int32 Y, struct Rect * RectData, struct Color4ub * Color);

////////////////////////////////////////////////////////////
/// Delete all textures
////////////////////////////////////////////////////////////
void TextureManagerDestroy();

#endif // GRAPHICS_TEXTUREMANAGER_H
