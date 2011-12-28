////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Graphics/Texture.h>
#include <Graphics/OpenGLHelper.h>
#include <Graphics/TextureHelper.h>
#include <Math/General.h>
#include <System/IOHelper.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
/// Update correct size from GL_ARB
////////////////////////////////////////////////////////////
UInt32 TextureValidSize(UInt32 Size)
{
	// If hardware doesn't support NPOT textures, we calculate the nearest power of two
    if (!SupportsTextureNPOT)
		return MathPowerOfTwo(Size);
	
	// If hardware supports NPOT textures, then just return the unmodified size
	return Size;
}

////////////////////////////////////////////////////////////
/// Create 2D texture with loaded data
////////////////////////////////////////////////////////////
bool TextureCreate(struct Texture * TextureData, bool Mipmap, bool ColorKey)
{
	UInt32 TextureWidth, TextureHeight;
	GLint  PreviousTexture = 0;
	GLuint Texture = 0;

	if (!TextureData->Pixels)
		return false;

	// Check if texture parameters are valid before creating it
	if (!TextureData->Width || !TextureData->Height)
		return false;

    // Adjust internal texture dimensions depending on NPOT textures support
    TextureWidth  = TextureValidSize(TextureData->Width);
    TextureHeight = TextureValidSize(TextureData->Height);

    // Check the maximum texture size
    if ((TextureWidth > (UInt32)(TextureMaxSize)) || (TextureHeight > (UInt32)(TextureMaxSize)))
    {
        MessageError("TextureCreate", "Failed to create image, its internal size is too high (%dx%d).", TextureWidth, TextureHeight);
        return false;
    }

    TextureData->TextureWidth = TextureWidth;
    TextureData->TextureHeight = TextureHeight;

	glEnable(GL_TEXTURE_2D);

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &PreviousTexture);

	glGenTextures(1, &Texture);

	// Pixel storage mode (word alignment / 4 bytes)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glBindTexture(GL_TEXTURE_2D, Texture);

	// Color key
	if (ColorKey)
	{
		UInt32 i, TextureSize = TextureData->Width * TextureData->Height;;

		for (i = 0; i < TextureSize; i++)
		{
			if ( *(UInt32*)&TextureData->Pixels[i] == 0xFF000000 ) // { 0, 0, 0, 255 }
				TextureData->Pixels[i].a = 0;
		}
	}

	if (Mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, TextureData->TextureWidth, TextureData->TextureHeight,
										 GL_RGBA, GL_UNSIGNED_BYTE, &TextureData->Pixels[0]);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if (SupportsTextureNPOT != 0)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureData->TextureWidth, TextureData->TextureHeight,
												0, GL_RGBA, GL_UNSIGNED_BYTE, &TextureData->Pixels[0]);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureData->TextureWidth, TextureData->TextureHeight,
												0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TextureData->Width, TextureData->Height, GL_RGBA,
															GL_UNSIGNED_BYTE, &TextureData->Pixels[0]);
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	TextureData->TextureID = (UInt32)(Texture);

	glBindTexture(GL_TEXTURE_2D, PreviousTexture);

    return true;
}

////////////////////////////////////////////////////////////
/// Bind 2D texture with previous cache
////////////////////////////////////////////////////////////
void TextureBind(struct Texture * TextureData)
{
	GLint PreviousTexture;

	if (TextureData)
	{
		// Check texture
		if (TextureData->TextureID > 0)
		{
			glEnable(GL_TEXTURE_2D);

			glGetIntegerv(GL_TEXTURE_BINDING_2D, &PreviousTexture);

			if ( PreviousTexture != (UInt32)TextureData->TextureID )
			{
				// Bind the ID texture specified by the 2nd parameter
				glBindTexture(GL_TEXTURE_2D, TextureData->TextureID);
			}
		}
	}
}

////////////////////////////////////////////////////////////
/// Render a static 2D texture with triangle strip
/// Specifications: The texture doesn't depend on rect
///					the rect can cut it
////////////////////////////////////////////////////////////
void TextureRenderEx(struct Texture * TextureData, Int32 X, Int32 Y, struct Rect * RectData, struct Color4ub * Color)
{
	Int32 Width, Height;

	// Bind texture
	TextureBind(TextureData);

	// Calculate size
	Width = RectData->Right - RectData->Left;
	Height = RectData->Bottom - RectData->Top;

	// Blend
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Set color
	glColor4ub(Color->r, Color->g, Color->b, Color->a);

	// Render
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2i(0, 0);	glVertex2i(X, Y);
		glTexCoord2i(1, 0);	glVertex2i(X + Width, Y);
		glTexCoord2i(0, 1);	glVertex2i(X, Y + Height);
		glTexCoord2i(1, 1);	glVertex2i(X + Width, Y + Height);
	glEnd();
}

/*void TextureRenderVertexArray(struct Texture * TextureData, Int32 X, Int32 Y, struct Rect * RectData, struct Color4ub * Color)
{
	
	GLint	Coordinates[] = { 0, 0, 1, 0, 0, 1, 1, 1 };
	GLubyte Colors[] = { Color->r, Color->g, Color->b, Color->a,
						 Color->r, Color->g, Color->b, Color->a,
						 Color->r, Color->g, Color->b, Color->a,
						 Color->r, Color->g, Color->b, Color->a };

	Int32 Width, Height;

	// Bind texture
	TextureBind(TextureData);

	// Calculate size
	Width = RectData->Right - RectData->Left;
	Height = RectData->Bottom - RectData->Top;

	// Blend
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Enble vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Specify pointers to vertex arrays
    glNormalPointer(GL_UNSIGNED_INT, 0, Normals);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, Colors);
    glVertexPointer(3, GL_UNSIGNED_INT, 0, Vertices);
	glTexCoordPointer(2, GL_INT, 0, Coordinates);

    glPushMatrix();

	// Move to the position
    glTranslatef((GLfloat)X, (GLfloat)Y, 0);

	// Render
    glDrawArrays(GL_QUADS, 0, 24);

    glPopMatrix();

	// Disable vertex arrays
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}*/

////////////////////////////////////////////////////////////
/// Delete the texture from memory and OpenGL
////////////////////////////////////////////////////////////
void TextureDelete(struct Texture * TextureData)
{
	if (TextureData)
	{
		// Delete Texture
		glDeleteTextures(1, &TextureData->TextureID);

		// Delete memory
		Delete(TextureData->Pixels);
	}
}

////////////////////////////////////////////////////////////
/// Save texture from pixels as a bmp
////////////////////////////////////////////////////////////
bool TextureSave(char * Filename, Int32 Width, Int32 Height, Int32 Channels, UInt8 * Data)
{
	// Check integrity
	if( (Width < 1) || (Height < 1) ||
		(Channels < 1) || (Channels > 4) ||
		(Data == NULL) ||
		(Filename == NULL) )
	{
		return false;
	}

	return stbi_write_bmp( (char*)Filename, Width, Height, Channels, (void*)Data );
}

////////////////////////////////////////////////////////////
/// Get the free path to the screenshot
////////////////////////////////////////////////////////////
bool TextureFindScreenShotPath(char *Path)
{
	Int32 i;

	for (i = 0; i <= TEXTURE_SCREENSHOT_SIZE; i++)
	{
		FILE * File;

		sprintf(Path, "data/screenshots/%d.bmp", i);

		if (!fopen(&File, Path, "r")) return true;

		fclose(File);
	}

	return false;
}

////////////////////////////////////////////////////////////
/// Take a screenshot
////////////////////////////////////////////////////////////
void TextureScreenShot(Int32 Width, Int32 Height)
{
	char ScreenShotPath[32];
	UInt8 * Data;
	Int32 i, j;

	if (TextureFindScreenShotPath(ScreenShotPath) == false)
		return;

    // Get the data from OpenGL
    Data = (UInt8*)malloc( 3 * Width * Height );
    glReadPixels(0, 0, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, Data);

    // Invert the image
    for ( j = 0; j * 2 < Height; ++j )
	{
		Int32 Index1 = j * Width * 3;
		Int32 Index2 = (Height - 1 - j) * Width * 3;
		for (i = Width * 3; i > 0; --i )
		{
			UInt8 Temp = Data[Index1];
			Data[Index1] = Data[Index2];
			Data[Index2] = Temp;
			++Index1;
			++Index2;
		}
	}

    // Save the imgae
    if (TextureSave( ScreenShotPath, Width, Height, 3, Data) == false)
    {
		MessageError("Error", "Error when the application tried to save the screenshot.");
    }

    // And free the memory
    Delete(Data);
}
