////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/WindowManager.h>
#include <Gui/Clipboard.h>
#include <Gui/WindowParser.h>
#include <Game/Configuration.h>
#include <Math/Geometry/Rect.h>
#include <System/IOHelper.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct GuiWindowList	WindowList;				///< Window list
struct GuiWidget *		WidgetPressed = NULL;	///< Temp widget

////////////////////////////////////////////////////////////
/// Initialize the windows
////////////////////////////////////////////////////////////
void GuiInitialize()
{
	GuiEventHandlerInitialize();
	GuiParserInitialize(ConfigGet()->GuiTemplate);
}

////////////////////////////////////////////////////////////
// Add window to the list
////////////////////////////////////////////////////////////
void GuiAddWindow(struct GuiWindow * Window)
{
	if (WindowList.First == NULL)
	{
		WindowList.First = WindowList.Focused = Window;
		Window->Previous = NULL;
	}
	else
	{
		WindowList.Last->Next = Window;
		Window->Previous = WindowList.Last;
	}

	WindowList.Last = Window;
	Window->Next = NULL;
}

////////////////////////////////////////////////////////////
// Remove window from the list
////////////////////////////////////////////////////////////
void GuiRemoveWindow(struct GuiWindow * Window)
{
	if (Window->Previous == NULL)
		WindowList.First = Window->Next;
	else
		Window->Previous->Next = Window->Next;

	if (Window->Next == NULL)
		WindowList.Last = Window->Previous;
	else
		Window->Next->Previous = Window->Previous;

	GuiWindowDestroy(Window);
}

////////////////////////////////////////////////////////////
/// Get a window pointer by id
////////////////////////////////////////////////////////////
struct GuiWindow * GuiGetWindow(char * WindowID)
{
	struct GuiWindow * Iterator;

	for (Iterator = WindowList.First; Iterator != NULL; Iterator = Iterator->Next)
	{
		if (strncmp(Iterator->First->Text, WindowID, strlen(WindowID)) == 0)
			return Iterator;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
/// Open the window
////////////////////////////////////////////////////////////
void GuiOpenWindow(struct GuiWindow * Window)
{
	if (Window)
	{
		if (Window->hStatus != GUI_WINDOW_OPENED)
		{
			// Play sound
			SoundBufferPlay(GUI_FORM_SOUND_OPEN, false, 0, 0, 0);

			// Add to the list
			if (Window->hStatus != GUI_WINDOW_CLOSED)
                GuiAddWindow(Window);

			// Open the window
			Window->hStatus = GUI_WINDOW_OPENED;

			// Set as focused
			WindowList.Focused = Window;

			if (Window->First)
			{
				if (Window->First->Open)
					Window->First->Open();
			}
		}
	}
}

////////////////////////////////////////////////////////////
/// Load the window on the list
////////////////////////////////////////////////////////////
void GuiLoadWindow(struct GuiWindow * Window)
{
    if (Window)
    {
        // Set the status to hide it
		Window->hStatus = GUI_WINDOW_CLOSED;

		// Add to the list
        GuiAddWindow(Window);
    }
}

////////////////////////////////////////////////////////////
/// Close the window
////////////////////////////////////////////////////////////
void GuiCloseWindow(struct GuiWindow * Window)
{
	if (Window)
	{
		if (Window->hStatus != GUI_WINDOW_CLOSED)
		{
			// Close the window
			Window->hStatus = GUI_WINDOW_CLOSED;

			// Play sound
			SoundBufferPlay(GUI_FORM_SOUND_CLOSE, false, 0, 0, 0);

			if (Window->First)
			{
				if (Window->First->Close)
					Window->First->Close();
			}

			if (Window == WindowList.Focused)
			{
				struct GuiWindow * Iterator;

				// Set window focused
				for (Iterator = WindowList.First; Iterator != NULL; Iterator = Iterator->Next)
				{
					if (Iterator->hStatus == GUI_WINDOW_OPENED && Iterator != WindowList.Focused)
					{
						WindowList.Focused = Iterator;
						return;
					}
				}

				WindowList.Focused = NULL;
			}
		}
	}
}


////////////////////////////////////////////////////////////
// Render all gui (renderer)
////////////////////////////////////////////////////////////
void GuiRender()
{
	if (WindowList.First)
	{
		struct GuiWindow * Iterator;

		// Render all forms
		for (Iterator = WindowList.First; Iterator != NULL; Iterator = Iterator->Next)
		{
			if (Iterator == WindowList.Focused)
			{
				Iterator = Iterator->Next;

				if (!Iterator)
					break;
			}

			if (Iterator->hStatus == GUI_WINDOW_OPENED && Iterator != WindowList.Focused)
				GuiWindowRender(Iterator, false);
		}

		// Render focused window
		if (WindowList.Focused)
			GuiWindowRender(WindowList.Focused, true);
	}
}

////////////////////////////////////////////////////////////
// Update all gui (event handler)
////////////////////////////////////////////////////////////
void GuiUpdate(struct Event * EventData)
{
	struct GuiWidget * Iterator;
	bool EventComputed = false;

	// If there are any window
	if (WindowList.First && WindowList.Focused)
	{
		// Check events
		if ( EventData->Type )
		{
			switch ( EventData->Type )
			{
				// Mouse events
				case MouseMoved :
					if ( RectContainsEx(WindowList.Focused->First->X,
										WindowList.Focused->First->Y,
										WindowList.Focused->First->X + WindowList.Focused->First->Width,
										WindowList.Focused->First->Y + WindowList.Focused->First->Height,
									    EventData->MouseMove.X, EventData->MouseMove.Y) )
					{
						for (Iterator = WindowList.Focused->Last; Iterator != NULL; Iterator = Iterator->Previous)
						{
							if (Iterator->Type == GUI_WIDGET_BUTTON)
							{
								if (Iterator->Attributes.Button.ActionStatus != GUI_BUTTON_PRESSED)
									GuiButtonSetState(&Iterator->Attributes.Button, GUI_BUTTON_NORMAL);
							}

							if (EventComputed == false)
							{
								if ( RectContainsEx(WindowList.Focused->First->X + Iterator->X,
													WindowList.Focused->First->Y + Iterator->Y,
													WindowList.Focused->First->X + Iterator->X + Iterator->Width,
													WindowList.Focused->First->Y + Iterator->Y + Iterator->Height,
													EventData->MouseMove.X, EventData->MouseMove.Y) )
								{
									// Do the widget event
									if (Iterator->Type == GUI_WIDGET_BUTTON)
									{
										if (Iterator->Attributes.Button.ActionStatus != GUI_BUTTON_PRESSED)
											GuiButtonSetState(&Iterator->Attributes.Button, GUI_BUTTON_HOVERED);
									}

									if (Iterator->MouseMove)
										Iterator->MouseMove(&EventData->MouseMove);

									EventComputed = true;
								}
							}
						}
					}

					return;

				case MouseButtonPressed :
					if ( RectContainsEx(WindowList.Focused->First->X,
										WindowList.Focused->First->Y,
										WindowList.Focused->First->X + WindowList.Focused->First->Width,
										WindowList.Focused->First->Y + WindowList.Focused->First->Height,
									    EventData->MouseButton.X, EventData->MouseButton.Y) )
					{
						for (Iterator = WindowList.Focused->Last; Iterator != NULL; Iterator = Iterator->Previous)
						{
							if ( RectContainsEx(WindowList.Focused->First->X + Iterator->X,
												WindowList.Focused->First->Y + Iterator->Y,
												WindowList.Focused->First->X + Iterator->X + Iterator->Width,
												WindowList.Focused->First->Y + Iterator->Y + Iterator->Height,
												EventData->MouseButton.X, EventData->MouseButton.Y) )
							{
								// Do the widget event
								switch (Iterator->Type)
								{
									case GUI_WIDGET_BUTTON :
										SoundBufferPlay(GUI_BUTTON_SOUND_1, false, 0, 0, 0);
										GuiButtonSetState(&Iterator->Attributes.Button, GUI_BUTTON_PRESSED);
										break;

                                    case GUI_WIDGET_TEXTBOX :
                                        GuiTextBoxSetCurrentPos(Iterator, (EventData->MouseButton.X - WindowList.Focused->First->X));
                                        break;

									case GUI_WIDGET_LIST :
										GuiListSetItemSelected(Iterator,
											EventData->MouseButton.Y - WindowList.Focused->First->Y);
										break;

									default : break;
								}



								if (Iterator->ButtonPressed)
									Iterator->ButtonPressed(&EventData->MouseButton);

								WindowList.Focused->FocusWidget = WidgetPressed = Iterator;

								return;
							}
						}
					}
					else
					{
						struct GuiWindow * WindowItr;

						for (WindowItr = WindowList.First; WindowItr != NULL; WindowItr = WindowItr->Next)
						{
							if ( RectContainsEx(WindowItr->First->X,
												WindowItr->First->Y,
												WindowItr->First->X + WindowItr->First->Width,
												WindowItr->First->Y + WindowItr->First->Height,
												EventData->MouseButton.X, EventData->MouseButton.Y) )
							{
								WindowList.Focused = WindowItr;
								
								// Play sound
								SoundBufferPlay(GUI_FORM_SOUND_FOCUS, false, 0, 0, 0);
								return;
							}
						}
					}

					return;

				case MouseButtonReleased :
					if (WidgetPressed)
					{
						// Do the widget event
						if (WidgetPressed->Type == GUI_WIDGET_BUTTON)
							GuiButtonSetState(&WidgetPressed->Attributes.Button, GUI_BUTTON_HOVERED);

						if (WidgetPressed->ButtonReleased)
							WidgetPressed->ButtonReleased(&EventData->MouseButton);

						WidgetPressed = NULL;

						return;
					}

					return;


				// Keyboard events
				case TextEntered :
					// Send the event
					if (WindowList.Focused->FocusWidget->TextEntered)
						WindowList.Focused->FocusWidget->TextEntered(&EventData->Text);

					if (WindowList.Focused->FocusWidget->Type == GUI_WIDGET_TEXTBOX)
					{
						// Update the text
						if (EventData->Text.Unicode >= 0x20 && EventData->Text.Unicode < 0x7F)
						{
							GuiTextBoxAddChar(WindowList.Focused->FocusWidget, EventData->Text.Unicode);
							return;
						}

						if (EventData->Text.Unicode == 0x08)
						{
							GuiTextBoxRemoveChar(WindowList.Focused->FocusWidget);
							return;
						}
					}

					return;

				case KeyPressed :
					// Send the event
					if (WindowList.Focused->FocusWidget->KeyPressed)
						WindowList.Focused->FocusWidget->KeyPressed(&EventData->Key);

					return;

				case KeyReleased :
					// Send the event
					if (WindowList.Focused->FocusWidget->KeyReleased)
						WindowList.Focused->FocusWidget->KeyReleased(&EventData->Key);

					// Clipboard events
					if (EventData->Key.Control)
					{
						if (EventData->Key.Code == 'c' || EventData->Key.Code == 'C')
						{
							switch (WindowList.Focused->FocusWidget->Type)
							{
								case GUI_WIDGET_TEXTBOX :
									GuiTextBoxCopySelection(WindowList.Focused->FocusWidget);
									return;

								case GUI_WIDGET_LIST :
									GuiClipboardSet(GuiListGetItemSelected(&WindowList.Focused->FocusWidget->Attributes.List),
													strlen(GuiListGetItemSelected(&WindowList.Focused->FocusWidget->Attributes.List)));
									return;

								default :
									return;
							}
						}

						if (EventData->Key.Code == 'x' || EventData->Key.Code == 'X')
						{
							switch (WindowList.Focused->FocusWidget->Type)
							{
								case GUI_WIDGET_TEXTBOX :
									GuiTextBoxCutSelection(WindowList.Focused->FocusWidget);
									return;

								default :
									return;
							}
						}

						if (EventData->Key.Code == 'v' || EventData->Key.Code == 'V')
						{
							switch (WindowList.Focused->FocusWidget->Type)
							{
								case GUI_WIDGET_TEXTBOX :
									GuiTextBoxPasteSelection(WindowList.Focused->FocusWidget);
									return;

								default :
									return;
							}
						}
					}



					return;

				// Another else
				default : return;
			}
		}
	}
}

////////////////////////////////////////////////////////////
/// Destroy all windows
////////////////////////////////////////////////////////////
void GuiDestroy()
{
	struct GuiWindow * Iterator = WindowList.First;

	while (Iterator != NULL)
	{
		struct GuiWindow * Next = Iterator->Next;

		// Destroy it
		GuiWindowDestroy(Iterator);

		// Go to the next
		Iterator = Next;
	}

	// Set focused to null
	WindowList.Focused = NULL;

	// Destroy all events
	GuiEventHandlerDestroy();

	// Destroy the parser
	GuiParserDestroy();
}
