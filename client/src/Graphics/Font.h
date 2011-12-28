#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <ft2build.h>			///< FreeType library includes
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <Graphics/OpenGLHelper.h>
#include <Graphics/Color.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define FONT_LIST_SIZE	256
#define FONT_ERROR		-1

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
typedef struct FontTexture		///< Type definition of Font texture
{
	float pt, * wids, * hoss;
	Int32 * qvws, * qvhs;
	float * qtws, * qths;
	UInt32 * textures;
	float ascend;
} FontTexture;

typedef struct FontGlyph		///< Type definition of Font glyph
{
	float pt;
	FT_Face face;
} FontGlyph;

typedef struct Font				///< Type definition of Font
{
	FontTexture * Texture;
	FontGlyph	* Glyph;
	bool		  Loaded;
} FontData;

FontData FontList[FONT_LIST_SIZE];	///< Font database

////////////////////////////////////////////////////////////
/// Initialize freetype library definition
///
////////////////////////////////////////////////////////////
bool FontInitialize();

////////////////////////////////////////////////////////////
/// Destroy freetype library definition
///
////////////////////////////////////////////////////////////
void FontDestroy();

////////////////////////////////////////////////////////////
/// Load a font from path
///
////////////////////////////////////////////////////////////
FontGlyph * FontGlyphLoad(char * Filename, int Size);

////////////////////////////////////////////////////////////
/// Load a font from glyph
///
////////////////////////////////////////////////////////////
FontTexture * FontTextureFromGlyph(FontGlyph * Font);

////////////////////////////////////////////////////////////
/// Destroy a font glyph
///
////////////////////////////////////////////////////////////
void FontGlyphDestroy(FontGlyph * Font);

////////////////////////////////////////////////////////////
/// Destroy a font texture
///
////////////////////////////////////////////////////////////
void FontTextureDestroy(FontTexture * Font);

////////////////////////////////////////////////////////////
/// Load font
///
////////////////////////////////////////////////////////////
Int32 FontLoad(char * Path, UInt32 Size);

////////////////////////////////////////////////////////////
/// Render the text from the font
///
////////////////////////////////////////////////////////////
void FontRender(FontTexture * Font, float x, float y, char * Text, GLubyte * Color);

////////////////////////////////////////////////////////////
/// Render the text
///
////////////////////////////////////////////////////////////
void FontRenderTextEx(Int32 FontIndex, float X, float Y, char * Text, struct Color4ub * C);

////////////////////////////////////////////////////////////
/// Render the text (without color and font specification)
///
////////////////////////////////////////////////////////////
void FontRenderText(float X, float Y, char * Text);

////////////////////////////////////////////////////////////
/// Render the text (without index specification)
////////////////////////////////////////////////////////////
void FontRenderTextColor(float X, float Y, char * Text, struct Color4ub * Color);

#endif // GRAPHICS_FONT_H