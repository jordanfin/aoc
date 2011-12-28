////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/WindowObj.h>

////////////////////////////////////////////////////////////
// Internal data
////////////////////////////////////////////////////////////
static struct Color4ub ColorDisabled	= { 127, 127, 127, 255 };
static struct Color4ub ColorNormal		= { 255, 255, 255, 255 };

////////////////////////////////////////////////////////////
// Add widget to window
////////////////////////////////////////////////////////////
void GuiWindowAddWidget(struct GuiWindow * Window, struct GuiWidget * Widget)
{
	if (Window->First == NULL)
	{
		Window->First = Widget;
		Widget->Previous = NULL;
	}
	else
	{
		Window->Last->Next = Widget;
		Widget->Previous = Window->Last;
	}

	Window->FocusWidget = Window->Last = Widget;
	Widget->Next = NULL;
}

////////////////////////////////////////////////////////////
/// Remove widget to window
////////////////////////////////////////////////////////////
void GuiWindowRemoveWidget(struct GuiWindow * Window, struct GuiWidget * Widget)
{
	if (Widget->Previous == NULL)
		Window->First = Widget->Next;
	else
		Widget->Previous->Next = Widget->Next;

	if (Widget->Next == NULL)
		Window->Last = Widget->Previous;
	else
		Widget->Next->Previous = Widget->Previous;

	GuiWidgetDestroy(Widget);
}

////////////////////////////////////////////////////////////
/// Create the window
////////////////////////////////////////////////////////////
struct GuiWindow * GuiWindowCreate()
{
	// Set status and pointers
	struct GuiWindow * Window = (struct GuiWindow *)malloc(sizeof(struct GuiWindow));

	if (Window)
	{
		Window->hStatus		= GUI_WINDOW_NOTLOADED;
		Window->First = NULL;
		Window->Last  = NULL;
		Window->FocusWidget = NULL;

		return Window;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
/// Destroy the window
////////////////////////////////////////////////////////////
void GuiWindowDestroy(struct GuiWindow * Window)
{
	if (Window)
	{
		struct GuiWidget * Iterator = Window->First;

		while (Iterator != NULL)
		{
			struct GuiWidget * Next = Iterator->Next;

			// Destroy it
			GuiWidgetDestroy(Iterator);

			// Go to the next
			Iterator = Next;
		}

		// Set focused to null
		Window->FocusWidget = NULL;

		// Set status
		Delete(Window);
	}
}

////////////////////////////////////////////////////////////
/// Render a form
////////////////////////////////////////////////////////////
void GuiWindowRender(struct GuiWindow * Window, bool Focused)
{
	struct GuiWidget * Iterator;
	struct Color4ub Color;

	for (Iterator = Window->First; Iterator != NULL; Iterator = Iterator->Next)
	{
		// Update color
		if (!Iterator->Enabled)
			memcpy(&Color, &ColorDisabled, sizeof(struct Color4ub));
		else
			memcpy(&Color, &ColorNormal, sizeof(struct Color4ub));

		if (Focused)
			Color.a = 255;
		else
			Color.a = 127;

		// Render widget
		switch (Iterator->Type)
		{
			case GUI_WIDGET_FORM :
				// Render the background repeating the texture
				GuiFormRender(Iterator, &Color);
				break;

			case GUI_WIDGET_BUTTON :
				// Render the texture, render the text centred with color
				GuiButtonRender(Window->First, Iterator, &Color);
				break;

			case GUI_WIDGET_TEXTBOX :
				// Render text
				GuiTextBoxRender(Window->First, Iterator, &Color, (Iterator == Window->FocusWidget));
				break;

			case GUI_WIDGET_CONSOLE :
				// Render

				break;

			case GUI_WIDGET_SCROLL :
				// Render

				break;

			case GUI_WIDGET_LABEL :
				// Render label
				GuiLabelRender(Window->First, Iterator, &Color);
				break;

			case GUI_WIDGET_LIST :
				// Render the list
				GuiListRender(Window->First, Iterator, &Color);
				break;

			default : break;
		}
	}
}

////////////////////////////////////////////////////////////
/// Return the state
////////////////////////////////////////////////////////////
UInt32 GuiWindowStatus(struct GuiWindow * Window)
{
	if (!Window)
		return GUI_WINDOW_NOTLOADED;

	return Window->hStatus;
}
