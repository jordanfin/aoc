////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/Button.h>
#include <Graphics/TextureRender.h>
#include <Graphics/Font.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Create a button
////////////////////////////////////////////////////////////
struct GuiWidget * GuiButtonCreate(Int32 X, Int32 Y, UInt32 FontSize, char * Text)
{
	struct GuiWidget * Widget;

	// Create
	Widget = GuiWidgetCreate(GUI_WIDGET_BUTTON, X, Y, FontSize, Text);

	if (Widget)
	{
		// Set the rect
		IndexGetSize(GUI_BUTTON_NORMAL_TEXTURE, &Widget->Width, &Widget->Height);

		// Set the status
		GuiButtonSetState(&Widget->Attributes.Button, GUI_BUTTON_NORMAL);

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Create a button by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiButtonCreateDefault()
{
	// Create
	struct GuiWidget * Widget = GuiWidgetCreateDefault(GUI_WIDGET_BUTTON);

	if (Widget)
	{
		// Set the rect (get the width and height from the grh)
		IndexGetSize(GUI_BUTTON_NORMAL_TEXTURE, &Widget->Width, &Widget->Height);

		// Set the status
		GuiButtonSetState(&Widget->Attributes.Button, GUI_BUTTON_NORMAL);

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Set state of the button
////////////////////////////////////////////////////////////
void GuiButtonSetState(struct GuiButton * Button, Int32 State)
{
	Button->ActionStatus = State;
}

////////////////////////////////////////////////////////////
// Render a button
////////////////////////////////////////////////////////////
void GuiButtonRender(struct GuiWidget * Form, struct GuiWidget * Widget, struct Color4ub * Color)
{
	struct GuiButton *	Button = &Widget->Attributes.Button;
	Int32				Texture;
	Int32				TextX, TextY, OffsetY = 0;

	// Render
	if (Button->ActionStatus == GUI_BUTTON_HOVERED)
	{
		Texture = GUI_BUTTON_HOVERED_TEXTURE;
	}
	else if (Button->ActionStatus == GUI_BUTTON_PRESSED)
	{
		OffsetY = 2;
		Texture = GUI_BUTTON_PRESSED_TEXTURE;
	}
	else
	{
		Texture = GUI_BUTTON_NORMAL_TEXTURE;
	}

	TextureRenderGrhColor(Texture,
						  Form->X + Widget->X,
						  Form->Y + Widget->Y,
						  Color);

	if (Widget->Text[0] == NULLCHAR)
		return;

	// Calculate center position
	TextX = (Widget->Width / 2) - (Widget->FontSize * (Widget->Length / 2));
	TextY = (Widget->Height / 2) - 4 + OffsetY;

	FontRenderTextColor((float)(TextX + Widget->X + Form->X), (float)(TextY + Widget->Y + Form->Y), Widget->Text, Color);
}
