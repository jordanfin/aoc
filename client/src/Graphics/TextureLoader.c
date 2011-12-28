////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Graphics/TextureLoader.h>
#include <Graphics/TextureHelper.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
/// Load pixels from an image file
////////////////////////////////////////////////////////////
bool TextureLoadFromFile( char * Filename, struct Texture * TextureData )
{
    // Load the image and get a pointer to the pixels in memory
    Int32 Width, Height, Channels;
    UInt8 * PixelsPtr = stbi_load_from_file(Filename, &Width, &Height, &Channels, TEXTURE_LOAD_RGBA);

    if (PixelsPtr)
    {
        // Copy the loaded pixels to the pixel buffer
		TextureData->Pixels = (struct Color4ub *)(malloc(Width * Height * sizeof(struct Color4ub)));
        memcpy(TextureData->Pixels, PixelsPtr, Width * Height * sizeof(struct Color4ub));

        TextureData->Width = Width;
        TextureData->Height = Height;

        // Free the loaded pixels (they are now in our own pixel buffer)
        Delete(PixelsPtr);

        return true;
    }
    else
    {
        // Error, failed to load the image
		MessageError("TextureLoadFromFile", "Failed to load image %s. Reason: %s.", Filename, stbi_failure_reason());
        return false;
    }
}

////////////////////////////////////////////////////////////
/// Load pixels from an image file
////////////////////////////////////////////////////////////
bool TextureLoadFromFileEx( char * Filename, struct Texture * TextureData, struct Rect * SubTexture )
{
    // Load the image and get a pointer to the pixels in memory
    Int32 Width, Height, Channels;
    
	UInt8 * PixelsPtr = stbi_load_from_file(Filename, &Width, &Height, &Channels, TEXTURE_LOAD_RGBA);

    if (PixelsPtr)
    {
		Int32 x0, x1, y0, y1;
		Int32 SubWidth = SubTexture->Right - SubTexture->Left;
		Int32 SubHeight = SubTexture->Bottom - SubTexture->Top;

        // Copy the loaded pixels to the pixel buffer
		TextureData->Pixels = (struct Color4ub *)(malloc(SubWidth * SubHeight * sizeof(struct Color4ub)));
        
		for (x0 = 0, x1 = (SubTexture->Left * 4); x0 < SubWidth && x1 < (Width * 4); x0++, x1+=4)
		{
			for (y0 = 0, y1 = (SubTexture->Top * 4); y0 < SubHeight && y1 < (Height * 4); y0++, y1+=4)
			{
				UInt32 Position0 = x0 + y0 * SubWidth;
				UInt32 Position1 = x1 + y1 * Width;

				TextureData->Pixels[Position0].r = PixelsPtr[Position1];
				TextureData->Pixels[Position0].g = PixelsPtr[Position1 + 1];
				TextureData->Pixels[Position0].b = PixelsPtr[Position1 + 2];
				TextureData->Pixels[Position0].a = PixelsPtr[Position1 + 3];
			}
		}

        TextureData->Width = SubWidth;
        TextureData->Height = SubHeight;

        // Free the loaded pixels (they are now in our own pixel buffer)
        Delete(PixelsPtr);

        return true;
    }
    else
    {
        // Error, failed to load the image
		MessageError("TextureLoadFromFile", "Failed to load image %s. Reason: %s.", Filename, stbi_failure_reason());
        return false;
    }
}

////////////////////////////////////////////////////////////
/// Load pixels from an image file in memory
////////////////////////////////////////////////////////////
bool TextureLoadFromMemory(UInt8 * Data, Int32 SizeInBytes, struct Texture * TextureData )
{
    // Load the image and get a pointer to the pixels in memory
    Int32 Width, Height, Channels;
    UInt8 * PixelsPtr = stbi_load_from_memory(Data, SizeInBytes, &Width, &Height, &Channels, TEXTURE_LOAD_RGBA);

    if (PixelsPtr)
    {

        // Copy the loaded pixels to the pixel buffer
        TextureData->Pixels = (struct Color4ub *)(malloc(Width * Height * sizeof(struct Color4ub)));
        memcpy(TextureData->Pixels, PixelsPtr, Width * Height * sizeof(struct Color4ub));

        TextureData->Width = Width;
        TextureData->Height = Height;

        // Free the loaded pixels (they are now in our own pixel buffer)
        Delete(PixelsPtr);

        return true;
    }
    else
    {
        // Error, failed to load the image
		MessageError("TextureLoadFromFile", "Failed to load image from memory. Reason: %s", stbi_failure_reason());
        return false;
    }
}
