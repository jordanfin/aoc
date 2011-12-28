////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/TextBox.h>
#include <Gui/Clipboard.h>
#include <Graphics/TextureRender.h>
#include <Graphics/Font.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Create a textbox
////////////////////////////////////////////////////////////
struct GuiWidget * GuiTextBoxCreate(Int32 X, Int32 Y, Int32 Length, UInt32 FontSize, char * Text, bool Hide)
{
	struct GuiWidget * Widget;

	// Create
	Widget = GuiWidgetCreate(GUI_WIDGET_TEXTBOX, X, Y, FontSize, Text);

	if (Widget)
	{
		struct GuiTextBox * TextBox = &Widget->Attributes.TextBox;

		// Set the size
		Widget->Width			= (FontSize * Length);
		Widget->Height			= (FontSize + 2);

		// Set the text manager
		Widget->Length			= strlen(Text);
		TextBox->SelectionPos	= GUI_TEXTBOX_NOT_SELECTED;
		TextBox->CurrentPos		= Widget->Length;
		TextBox->StartOffset	= 0;
		TextBox->HideText		= Hide;
		// TextBox->Selected		= false;

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Create a textbox by default
////////////////////////////////////////////////////////////
struct GuiWidget * GuiTextBoxCreateDefault()
{
	// Create
	struct GuiWidget * Widget = GuiWidgetCreateDefault(GUI_WIDGET_TEXTBOX);

	if (Widget)
	{
		struct GuiTextBox * TextBox = &Widget->Attributes.TextBox;

		// Set the size
		Widget->Width			= 0;
		Widget->Height			= 0;

		// Set the text manager
		TextBox->SelectionPos	= GUI_TEXTBOX_NOT_SELECTED;
		TextBox->StartOffset	= 0;
		TextBox->CurrentPos		= 0;
		TextBox->HideText		= false;
		// TextBox->Selected		= false;

		return Widget;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
// Clears the selection (not erase it!!)
////////////////////////////////////////////////////////////
void GuiTextBoxClearSelection(struct GuiWidget * Widget)
{
	Widget->Attributes.TextBox.SelectionPos	= GUI_TEXTBOX_NOT_SELECTED;
}

////////////////////////////////////////////////////////////
// Erase the selection
////////////////////////////////////////////////////////////
void GuiTextBoxEraseSelection(struct GuiWidget * Widget)
{
	struct GuiTextBox * TextBox = &Widget->Attributes.TextBox;

	if (TextBox->SelectionPos >= 0)
	{
		// Update the text
		memcpy(&Widget->Text[TextBox->SelectionPos], &Widget->Text[TextBox->CurrentPos], (Widget->Length - (TextBox->CurrentPos - TextBox->SelectionPos)));

		GuiTextBoxClearSelection(Widget);
	}
}

////////////////////////////////////////////////////////////
// Set the selection
////////////////////////////////////////////////////////////
void GuiTextBoxSetSelection(struct GuiWidget * Widget, UInt32 Start, UInt32 CurPos)
{
	if (Start > Widget->Length || CurPos > Widget->Length)
		return;

	if (Start > CurPos)
	{
		// Swap values
		Widget->Attributes.TextBox.SelectionPos	= CurPos;
		Widget->Attributes.TextBox.CurrentPos	= Start;
	}
	else
	{
		Widget->Attributes.TextBox.SelectionPos	= Start;
		Widget->Attributes.TextBox.CurrentPos	= CurPos;
	}
}

////////////////////////////////////////////////////////////
// Copy the textbox selection
////////////////////////////////////////////////////////////
void GuiTextBoxCopySelection(struct GuiWidget * Widget)
{
	if (Widget->Attributes.TextBox.SelectionPos < 0)
		return;

	// Copy it
	GuiClipboardSet(&Widget->Text[Widget->Attributes.TextBox.SelectionPos], Widget->Attributes.TextBox.CurrentPos - Widget->Attributes.TextBox.SelectionPos + 1);
}

////////////////////////////////////////////////////////////
// Cut the textbox selection
////////////////////////////////////////////////////////////
void GuiTextBoxCutSelection(struct GuiWidget * Widget)
{
	GuiTextBoxCopySelection(Widget);
	GuiTextBoxEraseSelection(Widget);
}

////////////////////////////////////////////////////////////
// Paste the textbox copy
////////////////////////////////////////////////////////////
void GuiTextBoxPasteSelection(struct GuiWidget * Widget)
{
	// Paste it
	strcat(&Widget->Text[0], &GuiClipboardGet()[0]);

	// TODO: this functions, rendering of selection, keyboard and mouse checking
}

////////////////////////////////////////////////////////////
// Add character
////////////////////////////////////////////////////////////
void GuiTextBoxAddChar(struct GuiWidget * Widget, Int8 Char)
{
	struct GuiTextBox * TextBox = &Widget->Attributes.TextBox;

	if (Widget->Length == GUI_TEXTBOX_LENGTH)
		return;

	if (TextBox->CurrentPos != Widget->Length)
	{
		UInt32 i;

		for (i = Widget->Length; i > TextBox->CurrentPos; i--)
		{
			Widget->Text[i+1] = Widget->Text[i];
		}

	}

	Widget->Text[TextBox->CurrentPos] = Char;
	Widget->Length++;
	Widget->Text[Widget->Length] = NULLCHAR;
	TextBox->CurrentPos++;

	if ( ((TextBox->CurrentPos * (Widget->FontSize - 1))) > (UInt32)Widget->Width )
		TextBox->StartOffset++;
}

////////////////////////////////////////////////////////////
// Remove character
////////////////////////////////////////////////////////////
void GuiTextBoxRemoveChar(struct GuiWidget * Widget)
{
	struct GuiTextBox * TextBox = &Widget->Attributes.TextBox;

	if (TextBox->CurrentPos <= 0)
		return;

	if (TextBox->CurrentPos != Widget->Length)
	{
		UInt32 i;

		for (i = TextBox->CurrentPos; i < Widget->Length; i++)
		{
			Widget->Text[i] = Widget->Text[i+1];
		}
	}

	TextBox->CurrentPos--;
	Widget->Length--;
	Widget->Text[Widget->Length] = NULLCHAR;

	if (TextBox->StartOffset > 0)
		TextBox->StartOffset--;

}

////////////////////////////////////////////////////////////
// Set current position
////////////////////////////////////////////////////////////
void GuiTextBoxSetCurrentPos(struct GuiWidget * Widget, Int32 Position)
{
    Int32 Pos = (Position - Widget->X + IndexGetGrh(GUI_TEXTBOX_VBORDER)->PixelWidth) / Widget->FontSize;

	if (Pos >= 0 && Pos <= (Int32)Widget->Length)
        Widget->Attributes.TextBox.CurrentPos = Pos;
}

////////////////////////////////////////////////////////////
// Render a textbox
////////////////////////////////////////////////////////////
void GuiTextBoxRender(struct GuiWidget * Form, struct GuiWidget * Widget, struct Color4ub * Color, bool Focused)
{
	struct GuiTextBox * TextBox = &Widget->Attributes.TextBox;
	char				Temp[GUI_TEXTBOX_LENGTH];

	// Render the background repeating it
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_BACKGROUND,
								Form->X + Widget->X, Form->Y + Widget->Y,
								Form->X + Widget->X + Widget->Width + 2, Form->Y + Widget->Y + Widget->Height,
								Color);

	// Render the borders repeating it
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_VBORDER,
								Form->X + Widget->X - IndexGetGrh(GUI_TEXTBOX_VBORDER)->PixelWidth, Form->Y + Widget->Y,
								Form->X + Widget->X, Form->Y + Widget->Y + Widget->Height,
								Color);
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_VBORDER + 0x01,
								Form->X + Widget->X + Widget->Width + 2, Form->Y + Widget->Y,
								Form->X + Widget->X + Widget->Width + IndexGetGrh(GUI_TEXTBOX_VBORDER + 0x01)->PixelWidth + 2, Form->Y + Widget->Y + Widget->Height,
								Color);
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_HBORDER,
								Form->X + Widget->X, Form->Y + Widget->Y - IndexGetGrh(GUI_TEXTBOX_HBORDER)->PixelHeight,
								Form->X + Widget->X + Widget->Width + 2, Form->Y + Widget->Y,
								Color);
	TextureRenderGrhColorRepeat(GUI_TEXTBOX_HBORDER + 0x01,
								Form->X + Widget->X, Form->Y + Widget->Y + Widget->Height,
								Form->X + Widget->X + Widget->Width + 2, Form->Y + Widget->Y + Widget->Height + IndexGetGrh(GUI_TEXTBOX_HBORDER + 0x01)->PixelHeight,
								Color);

	// Render the edges
	TextureRenderGrhColor(GUI_TEXTBOX_EDGE, Form->X + Widget->X - IndexGetGrh(GUI_TEXTBOX_EDGE)->PixelWidth, Form->Y + Widget->Y - IndexGetGrh(GUI_TEXTBOX_EDGE)->PixelHeight, Color);
	TextureRenderGrhColor(GUI_TEXTBOX_EDGE + 0x01, Form->X + Widget->X + Widget->Width + 2, Form->Y + Widget->Y - IndexGetGrh(GUI_TEXTBOX_EDGE + 0x01)->PixelHeight, Color);
	TextureRenderGrhColor(GUI_TEXTBOX_EDGE + 0x02, Form->X + Widget->X - IndexGetGrh(GUI_TEXTBOX_EDGE + 0x02)->PixelWidth, Form->Y + Widget->Y + Widget->Height, Color);
	TextureRenderGrhColor(GUI_TEXTBOX_EDGE + 0x03, Form->X + Widget->X + Widget->Width + 2, Form->Y + Widget->Y + Widget->Height, Color);

	if (Widget->Text[0] != NULLCHAR)
	{
		strcpy(&Temp[0], &Widget->Text[TextBox->StartOffset]);

		// Render the selection
		if (Widget->Enabled && (TextBox->SelectionPos >= 0))
		{
			glColor4ub(Color->r, Color->g, Color->b, 255);

			glRecti(Form->X + Widget->X + ((TextBox->SelectionPos - TextBox->StartOffset) * (Widget->FontSize - 1)) + 2,
					Form->Y + Widget->Y,
					Form->X + Widget->X + ((TextBox->CurrentPos - TextBox->StartOffset) * (Widget->FontSize - 1)) + 2,
					Form->Y + Widget->Y + Widget->FontSize + 2);
		}

		if (!Widget->Attributes.TextBox.HideText)
		{
			FontRenderTextColor((float)(Widget->X + Form->X + 1),
								(float)(Widget->Y + Form->Y + 3), Temp, Color);
		}
		else
		{
			char * TextHide = (char*)malloc(Widget->Length + 1);
			memset(&TextHide[0], '*', (Widget->Length - TextBox->StartOffset));
			TextHide[Widget->Length] = NULLCHAR;
			FontRenderTextColor((float)(Widget->X + Form->X + 1),
								(float)(Widget->Y + Form->Y + 3), TextHide, Color);
			Delete(TextHide);
		}

		if (Widget->Enabled && Focused)
		{
			glBegin(GL_LINES);
				glVertex2i(Form->X + Widget->X + ((TextBox->CurrentPos - TextBox->StartOffset) * (Widget->FontSize - 1)), Form->Y + Widget->Y);
				glVertex2i(Form->X + Widget->X + ((TextBox->CurrentPos - TextBox->StartOffset) * (Widget->FontSize - 1)), Form->Y + Widget->Y + Widget->FontSize + 2);
			glEnd();
		}
	}
}
