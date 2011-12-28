////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/Form.h>
#include <Graphics/TextureRender.h>
#include <Window/Window.h>

////////////////////////////////////////////////////////////
// Create a form
////////////////////////////////////////////////////////////
struct GuiWidget * GuiFormCreate(UInt32 Style, Int32 X, Int32 Y, Int32 Width, Int32 Height, UInt32 FontSize, char * Title)
{
	struct GuiWidget * Widget;

	// Create
	Widget = GuiWidgetCreate(GUI_WIDGET_FORM, X, Y, FontSize, Title);

	if (Widget)
	{
		// Set the size
		Widget->Width	= Width;
		Widget->Height	= Height;

		if (X == GUI_FORM_CENTERED)
			Widget->X = (WindowWidth / 2) - ((Width + 16) / 2);

		if (Y == GUI_FORM_CENTERED)
			Widget->Y = (WindowHeight / 2) - ((Height + 16) / 2);

		// Set the style
		Widget->Attributes.Form.Background = Style;

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Create a form by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiFormCreateDefault()
{
	// Create
	struct GuiWidget * Widget = GuiWidgetCreateDefault(GUI_WIDGET_FORM);

	if (Widget)
	{
		// Set size and position
		Widget->Width	= 400;
		Widget->Height	= 400;
		Widget->X		= 16;
		Widget->Y		= 16;

		// Set the style
		Widget->Attributes.Form.Background = GUI_FORM_STYLE_1;

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Render a form
////////////////////////////////////////////////////////////
void GuiFormRender(struct GuiWidget * Widget, struct Color4ub * Color)
{
	// Render the background repeating it
	TextureRenderGrhColorRepeat(GUI_FORM_BACKGROUND + Widget->Attributes.Form.Background,
								Widget->X, Widget->Y,
								Widget->X + Widget->Width, Widget->Y + Widget->Height,
								Color);

	// Render the borders repeating it
	TextureRenderGrhColorRepeat(GUI_FORM_VBORDER,
								Widget->X - IndexGetGrh(GUI_FORM_VBORDER)->PixelWidth, Widget->Y,
								Widget->X, Widget->Y + Widget->Height,
								Color);
	TextureRenderGrhColorRepeat(GUI_FORM_VBORDER + 0x01,
								Widget->X + Widget->Width, Widget->Y,
								Widget->X + Widget->Width + IndexGetGrh(GUI_FORM_VBORDER + 0x01)->PixelWidth, Widget->Y + Widget->Height,
								Color);
	TextureRenderGrhColorRepeat(GUI_FORM_HBORDER,
								Widget->X, Widget->Y - IndexGetGrh(GUI_FORM_HBORDER)->PixelHeight,
								Widget->X + Widget->Width, Widget->Y,
								Color);
	TextureRenderGrhColorRepeat(GUI_FORM_HBORDER + 0x01,
								Widget->X, Widget->Y + Widget->Height,
								Widget->X + Widget->Width, Widget->Y + Widget->Height + IndexGetGrh(GUI_FORM_HBORDER + 0x01)->PixelHeight,
								Color);

	// Render the edges
	TextureRenderGrhColor(GUI_FORM_EDGE, Widget->X - IndexGetGrh(GUI_FORM_EDGE)->PixelWidth, Widget->Y - IndexGetGrh(GUI_FORM_EDGE)->PixelHeight, Color);
	TextureRenderGrhColor(GUI_FORM_EDGE + 0x01, Widget->X + Widget->Width, Widget->Y - IndexGetGrh(GUI_FORM_EDGE + 0x01)->PixelHeight, Color);
	TextureRenderGrhColor(GUI_FORM_EDGE + 0x02, Widget->X - IndexGetGrh(GUI_FORM_EDGE + 0x02)->PixelWidth, Widget->Y + Widget->Height, Color);
	TextureRenderGrhColor(GUI_FORM_EDGE + 0x03, Widget->X + Widget->Width, Widget->Y + Widget->Height, Color);
}
