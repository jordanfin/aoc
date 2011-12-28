////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Graphics/Font.h>
#include <System/Error.h>
#include <Math/General.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
FT_Library FreeTypeLib;			///< Library definition
UInt32 FontDefault;

////////////////////////////////////////////////////////////
/// Initialize freetype library definition
////////////////////////////////////////////////////////////
bool FontInitialize()
{
	Int32 i;

	if (!(FT_Init_FreeType(&FreeTypeLib)))
	{
		for (i = 0; i < FONT_LIST_SIZE - 1; i++)
		{
			FontList[i].Loaded = false;
		}

		FontDefault = FontLoad("data/fonts/font0.ttf", 5);

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////
/// Destroy freetype library definition
////////////////////////////////////////////////////////////
void FontDestroy()
{
	UInt32 i;

	for (i = 0; i < FONT_LIST_SIZE - 1; i++)
	{
		if (FontList[i].Loaded)
		{
			// Destroy texture font
			FontTextureDestroy(FontList[i].Texture);

			// Destroy glyph font
			FontGlyphDestroy(FontList[i].Glyph);
		}
	}

	FT_Done_FreeType(FreeTypeLib);
}

////////////////////////////////////////////////////////////
/// Load a font from path
////////////////////////////////////////////////////////////
FontGlyph * FontGlyphLoad(char * Filename, int Size)
{
	FontGlyph * Font = (FontGlyph*)malloc(sizeof(FontGlyph));

	// load the Font from the file
	if (FT_New_Face(FreeTypeLib, Filename, 0, &(Font->face)))
	{
		MessageError("FontGlyphLoad", "Failed when the program was trying load a Font: %s.", Filename);
		Delete(Font);
		return NULL;
	}

	// freetype measures Fonts in 64ths of pixels,
	// which 6 left bit shift multiplies the pt size by 64.
	FT_Set_Char_Size(Font->face, Size << 6, Size << 6, 96, 96);
	Font->pt = (float)Size;

	return Font;
}

////////////////////////////////////////////////////////////
/// Helper internal method to make glyph texture
////////////////////////////////////////////////////////////
bool MakeGlyphTexture(FontGlyph * gf, FontTexture * tf, unsigned char ch)
{
	register Int32 i, j;
	FT_Face face = gf->face;
	Int32 width, height;
	FT_Glyph glyph;
	FT_BitmapGlyph bitmap_glyph;
	GLubyte * expanded_data;
	FT_Bitmap bitmap;

	if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
		return false;

    if (FT_Get_Glyph(face->glyph, &glyph))
		return false;

	FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, true);

    bitmap_glyph = (FT_BitmapGlyph)glyph;

	bitmap = bitmap_glyph->bitmap;

	width = MathPowerOfTwo(bitmap.width);
	height = MathPowerOfTwo(bitmap.rows);

	expanded_data = (GLubyte*)malloc(sizeof(GLubyte)* 2 * width * height);

	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			expanded_data[2 * (i+j*width)] =
			expanded_data[2 * (i+j*width) + 1] =
			(i >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[i + bitmap.width * j];
		}
	}

    glBindTexture(GL_TEXTURE_2D, tf->textures[ch]);

	if (SupportsEdgeClamp)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA16, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);

    Delete(expanded_data);

	tf->wids[ch] = (float)(face->glyph->advance.x >> 6);
	tf->hoss[ch] = (float)((face->glyph->metrics.horiBearingY - face->glyph->metrics.height) >> 6);

	tf->qvws[ch] = bitmap.width;
	tf->qvhs[ch] = bitmap.rows;

	tf->qtws[ch] = (float)bitmap.width / (float)width;
	tf->qths[ch] = (float)bitmap.rows / (float)height;

	return true;
}

////////////////////////////////////////////////////////////
/// Load a font from glyph
////////////////////////////////////////////////////////////
FontTexture * FontTextureFromGlyph(FontGlyph * Font)
{
	register UInt32 i;
	FontTexture * tf = (FontTexture*)malloc(sizeof(FontTexture));

	tf->pt = Font->pt;

	// prepare the OpenGL textures / display lists
	tf->wids = (float*)malloc(sizeof(float)*255);
	tf->hoss = (float*)malloc(sizeof(float)*255);
	tf->qvws = (Int32*)malloc(sizeof(Int32)*255);
	tf->qvhs = (Int32*)malloc(sizeof(Int32)*255);
	tf->qtws = (float*)malloc(sizeof(float)*255);
	tf->qths = (float*)malloc(sizeof(float)*255);
	tf->textures = (UInt32*)malloc(sizeof(UInt32)*255);

	glGenTextures(255, tf->textures);

	for (i = 0; i < 255; i++)
	{
		if (!MakeGlyphTexture(Font, tf, (UInt8)i))
		{
			FontTextureDestroy(tf);
			return NULL;
		}
	}

	return tf;
}

////////////////////////////////////////////////////////////
/// Destroy a font glyph
////////////////////////////////////////////////////////////
void FontGlyphDestroy(FontGlyph * Font)
{
	if (Font != NULL)
	{
		FT_Done_Face(Font->face);
		Delete(Font);
	}
}

////////////////////////////////////////////////////////////
/// Destroy a font texture
////////////////////////////////////////////////////////////
void FontTextureDestroy(FontTexture * Font)
{
	if (Font != NULL)
	{
		glDeleteTextures(255, Font->textures);
		Delete(Font->wids);
		Delete(Font->textures);
		Delete(Font->qvws);
		Delete(Font->qvhs);
		Delete(Font->qtws);
		Delete(Font->qths);
		Delete(Font);
	}
}

////////////////////////////////////////////////////////////
/// Render the text from the font
////////////////////////////////////////////////////////////
void FontRender(FontTexture * Font, float x, float y, char * Text, GLubyte * Color)
{
	char * ch = Text;

	glPushAttrib(GL_LIST_BIT|GL_CURRENT_BIT|GL_ENABLE_BIT|GL_TRANSFORM_BIT);
	glMatrixMode(GL_MODELVIEW);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4ubv(Color);

	glPushMatrix();
	glScalef(1,-1,1);

	do
	{
		glPushMatrix();
		glTranslatef(x,-y - Font->pt-Font->hoss[(UInt32)*ch], 0);
		glBindTexture(GL_TEXTURE_2D, Font->textures[(UInt32)*ch]);

		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0,Font->qths[(UInt32)*ch]); glVertex2i(0,0);
			glTexCoord2i(0,0); glVertex2i(0,Font->qvhs[(UInt32)*ch]);
			glTexCoord2f(Font->qtws[(UInt32)*ch], Font->qths[(UInt32)*ch]); glVertex2i(Font->qvws[(UInt32)*ch],0);
			glTexCoord2f(Font->qtws[(UInt32)*ch],0); glVertex2i(Font->qvws[(UInt32)*ch],Font->qvhs[(UInt32)*ch]);
		glEnd();

		glPopMatrix();
		glTranslatef(Font->wids[(UInt32)*ch],0,0);
	} while (*ch++);

	glPopMatrix();
	glPopAttrib();
}

////////////////////////////////////////////////////////////
/// Load font
////////////////////////////////////////////////////////////
Int32 FontLoad(char * Path, UInt32 Size)
{
	Int32 i;

	for (i = 0; i < FONT_LIST_SIZE - 1; i++)
	{
		if (!FontList[i].Loaded)
		{
			// Load glyph font
			FontList[i].Glyph = FontGlyphLoad(Path, Size);

			// Load texture font
			FontList[i].Texture = FontTextureFromGlyph(FontList[i].Glyph);

			// Font loaded
			FontList[i].Loaded = true;

			return i;
		}
	}

	return FONT_ERROR;
}

////////////////////////////////////////////////////////////
/// Render the text
////////////////////////////////////////////////////////////
void FontRenderTextEx(Int32 FontIndex, float X, float Y, char * Text, struct Color4ub * C)
{
	GLubyte Color[4];

	if (FontIndex == FONT_ERROR)
		return;

	if (FontList[FontIndex].Loaded)
	{
		Color[0] = C->r;
		Color[1] = C->g;
		Color[2] = C->b;
		Color[3] = C->a;

		FontRender(FontList[FontIndex].Texture, X, Y, Text, Color);
	}
}

////////////////////////////////////////////////////////////
/// Render the text (without color specification)
////////////////////////////////////////////////////////////
void FontRenderText(float X, float Y, char * Text)
{
	// Color by default
	static struct Color4ub Color = { 255, 255, 255, 200 };

	// Render
	FontRenderTextEx(FontDefault, X, Y, Text, &Color);
}

////////////////////////////////////////////////////////////
/// Render the text (without index specification)
////////////////////////////////////////////////////////////
void FontRenderTextColor(float X, float Y, char * Text, struct Color4ub * Color)
{
	// Render
	FontRenderTextEx(FontDefault, X, Y, Text, Color);
}
