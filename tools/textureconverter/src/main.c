////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <SOIL/SOIL.h>
#include <libpng/png.h>

////////////////////////////////////////////////////////////
// Doc:
//	This is a simple program, which converts all bmp
//	graphics from a directory path to png format.
//	It changes key color pixels to max alpha value.
//	The program uses SOIL lib to read the texture in bmp
//	format and PNG lib to write in png format.
// Note:
//	All variables are hardcoded, because I don't looking
//	forward to make it configurable, I'm lazy.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////
static jmp_buf buf;

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define CONVERTER_PATH_INITIAL		"data/input"
#define CONVERTER_PATH_FINAL		"data/output"
#define CONVERTER_GRAPHICS_SIZE		25004

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Writes a png
////////////////////////////////////////////////////////////
BOOL PngWrite(char * Filename, UInt8 * Pixels, unsigned int Width, unsigned int Height);

////////////////////////////////////////////////////////////
// Application entry point
////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	char FilePath[256];
	UInt32 i;

	// Converter loop
	for (i = 0; i <= CONVERTER_GRAPHICS_SIZE; i++)
	{
		Int32 Width, Height, Channels;
		UInt8 *Pixels;

		// First, load the texture
		sprintf(FilePath, "%s/%d.bmp", CONVERTER_PATH_INITIAL, i);

		// Load it as a rgba, not to make any conversion
		Pixels = SOIL_load_image(FilePath, &Width, &Height, &Channels, SOIL_LOAD_RGBA);

		if (Pixels)
		{
			// Color key
			UInt32 j, TextureSize = Width * Height * 4;

			for ( j = 0; j < TextureSize; j += 4)
			{
				if ( *(UInt32*)&Pixels[j] == 0xFF000000 ) // { 0, 0, 0, 255 }
						Pixels[j + 3] = 0;
			}
			
			// Change the path
			sprintf(FilePath, "%s/%d.png", CONVERTER_PATH_FINAL, i);
			
			// Write as png format
			PngWrite(FilePath, Pixels, Width, Height);

			// Free pixel data
			SOIL_free_image_data(Pixels);

			// Print info
			printf("%d.png saved successfuly.\n", i);
		}
		//else
		//{
		//	printf("Texture %s not found or corrupted.\n", FilePath);
		//}
	}

	return 0;
}

////////////////////////////////////////////////////////////
// Handler to png errors
////////////////////////////////////////////////////////////
static void PngErrorHandler(png_structp Png, png_const_charp Message)
{
	printf("Failed to write PNG image : %s\n", Message);  
    longjmp(buf, 1);

}

////////////////////////////////////////////////////////////
// Writes a png
////////////////////////////////////////////////////////////
BOOL PngWrite(char * Filename, UInt8 * Pixels, unsigned int Width, unsigned int Height)
{
	png_structp	Png;
	png_infop	PngInfo;
	UInt32		i;
	png_byte	*PixelsPtr, **RowPointers;

	// Open the file to write in
	FILE * File = fopen(Filename, "wb");
	
	if (!File)
	{
		printf("Failed to save texture %s, cannot open file.\n", Filename);
		return FALSE;
	}
	
	// Create the main PNG structure
	Png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, &PngErrorHandler, NULL);
	if (!Png)
	{
		fclose(File);
		printf("Failed to save texture %s, cannot allocate PNG write structure.\n", Filename);
		return FALSE;
	}

	// Initialize the image informations
	PngInfo = png_create_info_struct(Png);
	if (!PngInfo)
	{
		fclose(File);
		png_destroy_write_struct(&Png, NULL);
		printf("Failed to save texture %s, cannot allocate PNG info structure.\n", Filename);
		return FALSE;
	}

	// For proper error handling...
	if (setjmp(buf))
	{
		png_destroy_write_struct(&Png, &PngInfo);
		return FALSE;
	}

	// Link the file to the PNG structure
	png_init_io(Png, File);

	// Set the image informations
	png_set_IHDR(Png, PngInfo, Width, Height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Write the header
	png_write_info(Png, PngInfo);

	// Get the pointers to the pixels rows into an array
	PixelsPtr = (png_byte*)&Pixels[0];
	RowPointers = (png_byte**)malloc(sizeof(png_byte*) * Height);
	for (i = 0; i < Height; ++i)
	{
		RowPointers[i] = PixelsPtr;
		PixelsPtr += Width * 4;
	}

	// Write pixels row by row
	png_set_rows(Png, PngInfo, RowPointers);
	png_write_png(Png, PngInfo, PNG_TRANSFORM_IDENTITY, NULL);

	// Finish writing the file
	png_write_end(Png, PngInfo);

	// Cleanup resources
	png_destroy_write_struct(&Png, &PngInfo);
	fclose(File);

	return TRUE;
}
