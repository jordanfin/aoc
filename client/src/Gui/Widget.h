#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Gui/Form.h>
#include <Gui/Button.h>
#include <Gui/TextBox.h>
#include <Gui/Label.h>
#include <Gui/List.h>
#include <Window/Event.h>

////////////////////////////////////////////////////////////
// Widget type definitions
////////////////////////////////////////////////////////////
#define GUI_WIDGET_DEFAULT				0x00

#define GUI_WIDGET_ERROR				0x00
#define GUI_WIDGET_FORM					0x01
#define GUI_WIDGET_BUTTON				0x02
#define GUI_WIDGET_TEXTBOX				0x03
#define GUI_WIDGET_CONSOLE				0x04
#define GUI_WIDGET_SCROLL				0x05
#define GUI_WIDGET_LABEL				0x06
#define GUI_WIDGET_LIST					0x07
#define GUI_WIDGET_IMAGE                0x08

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct GuiWidget
{
	char		Text[GUI_TEXTBOX_LENGTH];		///< Text
	UInt32		Length;							///< Length of characters in the text
	UInt32		FontSize;						///< Font size

	Int32		X, Y;							///< Position
	Int32		Width, Height;					///< Size

	bool		Enabled;						///< If it's available or not

	void (*KeyPressed)(struct KeyEvent*);
	void (*KeyReleased)(struct KeyEvent*);
	void (*TextEntered)(struct TextEvent*);
	void (*ButtonPressed)(struct MouseButtonEvent*);
	void (*ButtonReleased)(struct MouseButtonEvent*);
	void (*MouseMove)(struct MouseMoveEvent*);
	void (*Open)();
	void (*Close)();

	union										///< Widget data
	{
		struct GuiForm			Form;
		struct GuiButton		Button;
		struct GuiTextBox		TextBox;
		//struct GuiConsole		Console;		// TODO: We have to finish all widgets
		//struct GuiScroll		Scroll;
		//struct GuiCheckBox	CheckBox;
		//struct GuiImage		Image;
		struct GuiList			List;
	} Attributes;

	UInt32 Type;								///< Widget attribute type

	struct GuiWidget * Next;					///< Next widget data
	struct GuiWidget * Previous;				///< Previous widget data
};

////////////////////////////////////////////////////////////
// Alloc memory widget
////////////////////////////////////////////////////////////
struct GuiWidget * GuiWidgetCreate(Int32 Type, Int32 X, Int32 Y, UInt32 FontSize, char * Text);

////////////////////////////////////////////////////////////
// Create widget without allocating memory by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiWidgetCreateDefault(Int32 Type);

////////////////////////////////////////////////////////////
// Copy the events of the widget
////////////////////////////////////////////////////////////
void GuiWidgetCopyEvents(struct GuiWidget * Dest, struct GuiWidget * Src);

////////////////////////////////////////////////////////////
// Destroy widget and memory
////////////////////////////////////////////////////////////
void GuiWidgetDestroy(struct GuiWidget * Widget);

#endif // GUI_WIDGET_H
