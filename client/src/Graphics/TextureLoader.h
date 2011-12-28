#ifndef GRAPHICS_TEXTURELOADER_H
#define GRAPHICS_TEXTURELOADER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Graphics/Texture.h>

////////////////////////////////////////////////////////////
/// The format of images that may be loaded (force_channels).
/// TEXTURE_LOAD_AUTO leaves the image in whatever format it was found.
/// TEXTURE_LOAD_L forces the image to load as Luminous (greyscale)
/// TEXTURE_LOAD_LA forces the image to load as Luminous with Alpha
/// TEXTURE_LOAD_RGB forces the image to load as Red Green Blue
/// TEXTURE_LOAD_RGBA forces the image to load as Red Green Blue Alpha
////////////////////////////////////////////////////////////
enum
{
	TEXTURE_LOAD_AUTO	= 0,
	TEXTURE_LOAD_L		= 1,
	TEXTURE_LOAD_LA		= 2,
	TEXTURE_LOAD_RGB	= 3,
	TEXTURE_LOAD_RGBA	= 4
};

////////////////////////////////////////////////////////////
/// Load pixels from an image file
///
/// \param Filename : Path of image file to load
/// \param TextureData : Texture
///
/// \return true if loading was successful
///
////////////////////////////////////////////////////////////
bool TextureLoadFromFile( char * Filename, struct Texture * TextureData );

////////////////////////////////////////////////////////////
/// Load pixels from an image file
///
/// \param Filename : Path of image file to load
/// \param TextureData : Texture
/// \param SubTexture : The sub texture rect
///
/// \return true if loading was successful
///
////////////////////////////////////////////////////////////
bool TextureLoadFromFileEx( char * Filename, struct Texture * TextureData, struct Rect * SubTexture );

////////////////////////////////////////////////////////////
/// Load pixels from an image file in memory
///
/// \param Data :        Pointer to the file data in memory
/// \param SizeInBytes : Size of the data to load, in bytes
/// \param TextureData : Texture
///
/// \return true if loading was successful
///
////////////////////////////////////////////////////////////
bool TextureLoadFromMemory( UInt8 * Data, Int32 SizeInBytes, struct Texture * TextureData );

#endif // GRAPHICS_TEXTURELOADER_H
