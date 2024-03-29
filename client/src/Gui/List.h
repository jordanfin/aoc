#ifndef GUI_LIST_H
#define GUI_LIST_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct GuiList
{
	char	Items[0x20][0xFF];
	UInt32	Size;
	Int32	ItemSelected;
};

////////////////////////////////////////////////////////////
// Widget include
////////////////////////////////////////////////////////////
#include <Gui/Widget.h>

////////////////////////////////////////////////////////////
// Create a list
////////////////////////////////////////////////////////////
struct GuiWidget * GuiListCreate(Int32 X, Int32 Y, UInt32 Width, UInt32 Height, UInt32 FontSize);

////////////////////////////////////////////////////////////
// Create a list by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiListCreateDefault();

////////////////////////////////////////////////////////////
// Add an item to the list
////////////////////////////////////////////////////////////
void GuiListAddItem(struct GuiList * List, char * Item);

////////////////////////////////////////////////////////////
// Remove an item to the list
////////////////////////////////////////////////////////////
void GuiListRemoveItem(struct GuiList * List, char * Item);

////////////////////////////////////////////////////////////
// Set the item selected
////////////////////////////////////////////////////////////
void GuiListSetItemSelected(struct GuiWidget * Widget, UInt32 Position);

////////////////////////////////////////////////////////////
// Get the item selected from the list
////////////////////////////////////////////////////////////
char * GuiListGetItemSelected(struct GuiList * List);

////////////////////////////////////////////////////////////
// Render a list
////////////////////////////////////////////////////////////
void GuiListRender(struct GuiWidget * Form, struct GuiWidget * Widget, struct Color4ub * Color);

#endif // GUI_LIST_H