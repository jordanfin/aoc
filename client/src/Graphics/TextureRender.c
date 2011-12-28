////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Graphics/TextureRender.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureManager.h>
#include <Graphics/OpenGLHelper.h>
#include <Math/Geometry/Rect.h>
#include <Window/Window.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
static struct Color4ub ColorDefault = { 255, 255, 255, 255 };

////////////////////////////////////////////////////////////
/// Render a grh from index
/// (without color and rect specification)
////////////////////////////////////////////////////////////
void TextureRenderGrh(Int32 GrhID, Int32 X, Int32 Y)
{
    struct GrhData * GrhPtr = IndexGetGrh(GrhID);
	struct Rect RectData = { GrhPtr->sX, GrhPtr->sY,
							 GrhPtr->sX + GrhPtr->PixelWidth,
							 GrhPtr->sY + GrhPtr->PixelHeight };

	// Render
	TextureRender(GrhID, X, Y, &RectData, &ColorDefault);
}

////////////////////////////////////////////////////////////
/// Render a grh from index with alpha specification
/// (without rect specification)
////////////////////////////////////////////////////////////
void TextureRenderGrhA(Int32 GrhID, Int32 X, Int32 Y, UInt8 Alpha)
{
    struct Color4ub Color = { 255, 255, 255, Alpha };
    struct GrhData * GrhPtr = IndexGetGrh(GrhID);
	struct Rect RectData = { GrhPtr->sX, GrhPtr->sY,
							 GrhPtr->sX + GrhPtr->PixelWidth,
							 GrhPtr->sY + GrhPtr->PixelHeight };

	// Render
	TextureRender(GrhID, X, Y, &RectData, &Color);
}

////////////////////////////////////////////////////////////
/// Render a grh from index with color specification
/// (without rect specification)
////////////////////////////////////////////////////////////
void TextureRenderGrhColor(Int32 GrhID, Int32 X, Int32 Y, struct Color4ub * Color)
{
    struct GrhData * GrhPtr = IndexGetGrh(GrhID);
	struct Rect RectData = { GrhPtr->sX, GrhPtr->sY,
							 GrhPtr->sX + GrhPtr->PixelWidth,
							 GrhPtr->sY + GrhPtr->PixelHeight };
	// Render
	TextureRender(GrhID, X, Y, &RectData, Color);
}


////////////////////////////////////////////////////////////
/// Render a grh from index with color specification
/// (repeating it in all rect)
////////////////////////////////////////////////////////////
void TextureRenderGrhColorRepeat(Int32 GrhID, Int32 Left, Int32 Top, Int32 Right, Int32 Bottom, struct Color4ub * Color)
{
    struct GrhData * GrhPtr = IndexGetGrh(GrhID);
	struct Rect GrhRect = { GrhPtr->sX, GrhPtr->sY,
							GrhPtr->sX + GrhPtr->PixelWidth,
							GrhPtr->sY + GrhPtr->PixelHeight };

	// Load it, if it isn't loaded
	if (TextureLoad(GrhID, NULL, false, &GrhRect))
	{
		struct Texture * Tex = TextureGetData(GrhID);
		float CoordX, CoordY;

		Int32 Width = Right - Left;
		Int32 Height = Bottom - Top;

		// Bind texture
		TextureBind(Tex);

		// Calculate coordinates
		CoordX = Width / (float)Tex->TextureWidth;
		CoordY = Height / (float)Tex->TextureHeight;

		// Blend
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Set color
		glColor4ub(Color->r, Color->g, Color->b, Color->a);

		// Render
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0, 0);				glVertex2i(Left, Top);
			glTexCoord2f(CoordX, 0);		glVertex2i(Left + Width, Top);
			glTexCoord2f(0, CoordY);		glVertex2i(Left, Top + Height);
			glTexCoord2f(CoordX, CoordY);	glVertex2i(Left + Width, Top + Height);
		glEnd();
	}
}

////////////////////////////////////////////////////////////
/// Render a grh from pointer
/// (without color and rect specification)
////////////////////////////////////////////////////////////
void TextureRenderGrhPtr(struct GrhData * Grh, Int32 X, Int32 Y)
{
	// struct Rect RectData = { Grh->sX, Grh->sY,
	//						 Grh->sX + Grh->PixelWidth,
	//						 Grh->sY + Grh->PixelHeight };
	// Render
	// TextureRender(Grh->FileNumber, X, Y, &RectData, &ColorDefault);
}
