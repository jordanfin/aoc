////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/Button.h>
#include <Graphics/TextureRender.h>
#include <Graphics/Font.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Create a list
////////////////////////////////////////////////////////////
struct GuiWidget * GuiListCreate(Int32 X, Int32 Y, UInt32 Width, UInt32 Height, UInt32 FontSize)
{
	struct GuiWidget * Widget;

	// Create
	Widget = GuiWidgetCreate(GUI_WIDGET_LIST, X, Y, FontSize, NULL);

	if (Widget)
	{
		// Set the rect
		Widget->Width = Width;
		Widget->Height = Height;

		// Set the attributts
		Widget->Attributes.List.Size = 0;
		Widget->Attributes.List.ItemSelected = -1;

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Create a list by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiListCreateDefault()
{
	// Create
	struct GuiWidget * Widget = GuiWidgetCreateDefault(GUI_WIDGET_LIST);

	if (Widget)
	{
		// Set the rect
		Widget->Width = 120;
		Widget->Height = 300;

		// Set the attributts
		Widget->Attributes.List.Size = 0;
		Widget->Attributes.List.ItemSelected = -1;

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Add an item to the list
////////////////////////////////////////////////////////////
void GuiListAddItem(struct GuiList * List, char * Item)
{
	if (Item != NULL)
	{
		if (List->Size < 0x20)
		{
			strcpy(&List->Items[List->Size][0], Item);
			List->Size++;
		}
	}
}

////////////////////////////////////////////////////////////
// Remove an item to the list
////////////////////////////////////////////////////////////
void GuiListRemoveItem(struct GuiList * List, char * Item)
{
	if (Item != NULL)
	{
		UInt32 i;

		for (i = 0; i < List->Size; i++)
		{
			if (strcmp(&List->Items[i][0], Item) == 0)
			{
				List->Items[i][0] = NULLCHAR;
				List->Size--;
				return;
			}
		}
	}
}

////////////////////////////////////////////////////////////
// Set the item selected
////////////////////////////////////////////////////////////
void GuiListSetItemSelected(struct GuiWidget * Widget, UInt32 Position)
{
	UInt32 Item = ((Position / (Widget->FontSize + 3)) - 2);

	if (Item > 0x20 || Item >= Widget->Attributes.List.Size)
		return;

	if (Widget->Attributes.List.Items[Item][0] != NULLCHAR)
		Widget->Attributes.List.ItemSelected = Item;
}

////////////////////////////////////////////////////////////
// Get the item selected from the list
////////////////////////////////////////////////////////////
char * GuiListGetItemSelected(struct GuiList * List)
{
	if (List == NULL)
		return NULL;

	if (List->ItemSelected < 0)
		return NULL;

	return List->Items[List->ItemSelected];
}

////////////////////////////////////////////////////////////
// Render a button
////////////////////////////////////////////////////////////
void GuiListRender(struct GuiWidget * Form, struct GuiWidget * Widget, struct Color4ub * Color)
{
	struct GuiList * List = &Widget->Attributes.List;
	UInt32 i;

	// Render the background repeating it
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_BACKGROUND,
								Form->X + Widget->X, Form->Y + Widget->Y,
								Form->X + Widget->X + Widget->Width, Form->Y + Widget->Y + Widget->Height,
								Color);

	// Render the borders repeating it
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_VBORDER,
								Form->X + Widget->X - IndexGetGrh(GUI_TEXTBOX_VBORDER)->PixelWidth, Form->Y + Widget->Y,
								Form->X + Widget->X, Form->Y + Widget->Y + Widget->Height,
								Color);
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_VBORDER + 0x01,
								Form->X + Widget->X + Widget->Width, Form->Y + Widget->Y,
								Form->X + Widget->X + Widget->Width + IndexGetGrh(GUI_TEXTBOX_VBORDER + 0x01)->PixelWidth, Form->Y + Widget->Y + Widget->Height,
								Color);
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_HBORDER,
								Form->X + Widget->X, Form->Y + Widget->Y - IndexGetGrh(GUI_TEXTBOX_HBORDER)->PixelHeight,
								Form->X + Widget->X + Widget->Width, Form->Y + Widget->Y,
								Color);
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_HBORDER + 0x01,
								Form->X + Widget->X, Form->Y + Widget->Y + Widget->Height,
								Form->X + Widget->X + Widget->Width, Form->Y + Widget->Y + Widget->Height + IndexGetGrh(GUI_TEXTBOX_HBORDER + 0x01)->PixelHeight,
								Color);

	// Render the edges
	TextureRenderGrhColor(GUI_TEXTBOX_EDGE, Form->X + Widget->X - IndexGetGrh(GUI_TEXTBOX_EDGE)->PixelWidth, Form->Y + Widget->Y - IndexGetGrh(GUI_TEXTBOX_EDGE)->PixelHeight, Color);
	TextureRenderGrhColor(GUI_TEXTBOX_EDGE + 0x01, Form->X + Widget->X + Widget->Width, Form->Y + Widget->Y - IndexGetGrh(GUI_TEXTBOX_EDGE + 0x01)->PixelHeight, Color);
	TextureRenderGrhColor(GUI_TEXTBOX_EDGE + 0x02, Form->X + Widget->X - IndexGetGrh(GUI_TEXTBOX_EDGE + 0x02)->PixelWidth, Form->Y + Widget->Y + Widget->Height, Color);
	TextureRenderGrhColor(GUI_TEXTBOX_EDGE + 0x03, Form->X + Widget->X + Widget->Width, Form->Y + Widget->Y + Widget->Height, Color);

	if (List->ItemSelected >= 0)
	{
		UInt32 X, Y, Width, Height;

		X = Form->X + Widget->X;
		Y = Form->Y + Widget->Y + (List->ItemSelected * (Widget->FontSize + 3));
		Width = Widget->Width;
		Height = Widget->FontSize;

		// Render the rect
		glBegin(GL_TRIANGLE_STRIP);
			glVertex2i(X, Y);
			glVertex2i(X + Width, Y);
			glVertex2i(X, Y + Height);
			glVertex2i(X + Width, Y + Height);
		glEnd();
	}

	// Render the items
	for (i = 0; i < List->Size; i++)
	{
		FontRenderTextColor((float)(Widget->X + Form->X + 2),
							(float)(Widget->Y + Form->Y + 2 + ((Widget->FontSize + 3) * i)), List->Items[i], Color);
	}
}
