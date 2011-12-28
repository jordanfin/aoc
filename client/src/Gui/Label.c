////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/Label.h>
#include <Graphics/Font.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Create a label
////////////////////////////////////////////////////////////
struct GuiWidget * GuiLabelCreate(Int32 X, Int32 Y, UInt32 FontSize, char * Text)
{
	struct GuiWidget * Widget;

	// Create
	Widget = GuiWidgetCreate(GUI_WIDGET_LABEL, X, Y, FontSize, Text);

	if (Widget)
	{
		// Set the size
		Widget->Width = (FontSize * Widget->Length);
		Widget->Height = (FontSize + 2);

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Create a label by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiLabelCreateDefault()
{
	// Create
	struct GuiWidget * Widget = GuiWidgetCreateDefault(GUI_WIDGET_LABEL);

	if (Widget)
	{
		// Set the size
		Widget->Width = (Widget->FontSize * Widget->Length);
		Widget->Height = (Widget->FontSize + 2);

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Render a label
////////////////////////////////////////////////////////////
void GuiLabelRender(struct GuiWidget * Form, struct GuiWidget * Widget, struct Color4ub * Color)
{
	if (Widget->Text[0] == NULLCHAR)
		return;

	FontRenderTextColor((float)(Widget->X + Form->X + 2),
						(float)(Widget->Y + Form->Y + 3), Widget->Text, Color);
}
