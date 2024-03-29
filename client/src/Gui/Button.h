#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define GUI_BUTTON_SOUND_1				"data/audio/ui/click_1.ogg"
#define GUI_BUTTON_SOUND_2				"data/audio/ui/click_2.ogg"
#define GUI_BUTTON_NORMAL				0x00
#define GUI_BUTTON_PRESSED				0x01
#define GUI_BUTTON_HOVERED				0x02
#define GUI_BUTTON_NORMAL_TEXTURE		0x5C7F
#define GUI_BUTTON_PRESSED_TEXTURE		0x5C80
#define GUI_BUTTON_HOVERED_TEXTURE		0x5C81
#define GUI_BUTTON_NOT_TEXTURED			-1

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct GuiButton
{
	UInt32 ActionStatus;		///< Button event status
};

////////////////////////////////////////////////////////////
// Widget include
////////////////////////////////////////////////////////////
#include <Gui/Widget.h>

////////////////////////////////////////////////////////////
// Create a button
////////////////////////////////////////////////////////////
struct GuiWidget * GuiButtonCreate(Int32 X, Int32 Y, UInt32 FontSize, char * Text);

////////////////////////////////////////////////////////////
// Create a button by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiButtonCreateDefault();

////////////////////////////////////////////////////////////
// Set state of the button
////////////////////////////////////////////////////////////
void GuiButtonSetState(struct GuiButton * Button, Int32 State);

////////////////////////////////////////////////////////////
// Render a button
////////////////////////////////////////////////////////////
void GuiButtonRender(struct GuiWidget * Form, struct GuiWidget * Widget, struct Color4ub * Color);

#endif // GUI_BUTTON_H
