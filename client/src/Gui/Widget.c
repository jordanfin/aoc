////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/Widget.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////
// Alloc memory widget
////////////////////////////////////////////////////////////
struct GuiWidget * GuiWidgetCreate(Int32 Type, Int32 X, Int32 Y, UInt32 FontSize, char * Text)
{
	struct GuiWidget * Widget = (struct GuiWidget*)malloc(sizeof(struct GuiWidget));

	if (!Widget)
		return NULL;

	Widget->Enabled	= true;
	Widget->FontSize = FontSize;
	Widget->X = X;
	Widget->Y = Y;
	Widget->Close = Widget->Open = NULL;
	Widget->KeyPressed = NULL;
	Widget->KeyReleased = NULL;
	Widget->TextEntered = NULL;
	Widget->MouseMove = NULL;
	Widget->ButtonPressed = Widget->ButtonReleased = NULL;
	Widget->Next = Widget->Previous = NULL;
	Widget->Type = Type;

	// Set the text
	if (Text != NULL)
	{
		strcpy(Widget->Text, Text);
		Widget->Length = strlen(Text);
	}
	else
	{
		Widget->Text[0]	= NULLCHAR;
		Widget->Length = 0;
	}

	return Widget;
}

////////////////////////////////////////////////////////////
// Create widget without allocating memory by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiWidgetCreateDefault(Int32 Type)
{
	struct GuiWidget * Widget = (struct GuiWidget*)malloc(sizeof(struct GuiWidget));

	if (!Widget)
		return NULL;

	Widget->Enabled	= true;
	Widget->FontSize = 8;
	Widget->X = GUI_FORM_CENTERED;
	Widget->Y = GUI_FORM_CENTERED;
	Widget->Close = Widget->Open = NULL;
	Widget->KeyPressed = NULL;
	Widget->KeyReleased = NULL;
	Widget->TextEntered = NULL;
	Widget->MouseMove = NULL;
	Widget->ButtonPressed = Widget->ButtonReleased = NULL;
	Widget->Next = Widget->Previous = NULL;
	Widget->Type = Type;
	Widget->Text[0]	= NULLCHAR;
	Widget->Length = 0;

	return Widget;
}

////////////////////////////////////////////////////////////
// Copy the events of the widget
////////////////////////////////////////////////////////////
void GuiWidgetCopyEvents(struct GuiWidget * Dest, struct GuiWidget * Src)
{
	Dest->KeyPressed		= Src->KeyPressed;
	Dest->KeyReleased		= Src->KeyReleased;
	Dest->TextEntered		= Src->TextEntered;
	Dest->ButtonPressed		= Src->ButtonPressed;
	Dest->ButtonReleased	= Src->ButtonReleased;
	Dest->MouseMove			= Src->MouseMove;
	Dest->Open				= Src->Open;
	Dest->Close				= Src->Close;
}

////////////////////////////////////////////////////////////
// Destroy widget and memory
////////////////////////////////////////////////////////////
void GuiWidgetDestroy(struct GuiWidget * Widget)
{
    // Destroy widget
    Delete(Widget);
}
