////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Gui/WindowParser.h>
#include <Gui/Widget.h>
#include <Gui/WindowManager.h>
#include <System/String/Lexer.h>
#include <System/IOHelper.h>
#include <System/Error.h>
#include <Game/Configuration.h>
#include <Game/Language.h>
#include <Window/Window.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define GUI_PARSER_STATUS_ERROR				0x00	///< Parser error
#define GUI_PARSER_STATUS_EOF				0x01	///< The parser finished
#define GUI_PARSER_STATUS_PROCESSING		0x02	///< The parser is creating and adding the widget
#define GUI_PARSER_STATUS_READING_WIDGET	0x03	///< The parser is reading the widget type
#define GUI_PARSER_STATUS_READING_PARAMS	0x04	///< The parser is reading a widget params
#define GUI_PARSER_STATUS_WAITING			0x05	///< The parser is waiting for new data

#define GUI_PARSER_PROCESSING_ERROR			0x00	///< Processing error
#define GUI_PARSER_PROCESSING_SIMPLE		0x01	///< Processing a simple tag
#define GUI_PARSER_PROCESSING_OPENING		0x02	///< Processing an opening tag
#define GUI_PARSER_PROCESSING_CLOSING		0x03	///< Processing a closing tag

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct GuiWindow * ParserWindow	= NULL;							///< Parser window
struct GuiWidget * ParserWidget	= NULL;							///< Widget parsed
UInt32 ParserStatus				= GUI_PARSER_STATUS_WAITING;	///< Parser status indicator
UInt32 ProcessingStatus			= GUI_PARSER_PROCESSING_ERROR;	///< Processing status indicator
UInt32 WidgetType				= GUI_WIDGET_ERROR;				///< Type of the widget parsed
UInt32 ParameterType			= GUI_PARAMETER_ERROR;			///< Type of the parameter parsed

////////////////////////////////////////////////////////////
// Internal functions
////////////////////////////////////////////////////////////

static bool Lower(char * String, UInt32 Len)
{
	UInt32 i;

	for (i = 0; i < Len; i++)
	{
		if (!tolower(String[i]))
		{
			return false;
		}
	}

	return true;
}

static bool IsAlpha(char * String, UInt32 Len)
{
	UInt32 i;

	for (i = 0; i < Len; i++)
	{
		if (!isalpha(String[i]))
		{
			return false;
		}
	}

	return true;
}

static bool GetBool(char * str)
{
	if (strcmp(str, "true") == 0)
		return true;

	return false;
}

static bool CheckIntegrity(struct Lexer * Input)
{
	if (LexerOff(Input))
	{
		ParserStatus = GUI_PARSER_STATUS_EOF;

		return false;
	}
	else if (blength(LexerItemGet(Input)) == 0)
	{
		ParserStatus = GUI_PARSER_STATUS_ERROR;

		MessageError("GuiParserRead", "Invalid syntax (character: %d)", Input->Current);

		bdestroy(LexerItemGet(Input));

		return false;
	}

	return true;
}

static UInt32 GuiParserWidgetType(char * Type)
{
    if		(strcmp(Type, "form")		== 0)	return GUI_WIDGET_FORM;
    else if (strcmp(Type, "button")		== 0)	return GUI_WIDGET_BUTTON;
    else if (strcmp(Type, "textbox")	== 0)	return GUI_WIDGET_TEXTBOX;
	else if (strcmp(Type, "console")	== 0)	return GUI_WIDGET_CONSOLE;
	else if (strcmp(Type, "scroll")		== 0)	return GUI_WIDGET_SCROLL;
    else if (strcmp(Type, "label")		== 0)	return GUI_WIDGET_LABEL;
    else if (strcmp(Type, "list")		== 0)	return GUI_WIDGET_LIST;
    else if (strcmp(Type, "image")		== 0)	return GUI_WIDGET_IMAGE;
	
	return GUI_WIDGET_ERROR;
}

static UInt32 GuiParserParamType(char * Param)
{
    if		(strcmp(Param, "name") == 0)		return GUI_PARAMETER_TEXT;
    else if (strcmp(Param, "text") == 0)		return GUI_PARAMETER_TEXT;
    else if (strcmp(Param, "style") == 0)		return GUI_PARAMETER_STYLE;
    else if (strcmp(Param, "x") == 0)			return GUI_PARAMETER_X;
    else if (strcmp(Param, "y") == 0)			return GUI_PARAMETER_Y;
    else if (strcmp(Param, "height") == 0)		return GUI_PARAMETER_HEIGHT;
    else if (strcmp(Param, "width") == 0)		return GUI_PARAMETER_WIDTH;
    else if (strcmp(Param, "fontsize") == 0)	return GUI_PARAMETER_FONTSIZE;
	else if (strcmp(Param, "length") == 0)		return GUI_PARAMETER_LENGTH;
	else if (strcmp(Param, "hide") == 0)		return GUI_PARAMETER_HIDE;
    else if (strcmp(Param, "event") == 0)		return GUI_PARAMETER_EVENT;

	return GUI_PARAMETER_ERROR;
}

static void GuiParserCreateWidget()
{
	switch (WidgetType)
	{
		case GUI_WIDGET_FORM :
		{
			ParserWidget = GuiFormCreateDefault();
			break;
		}								

		case GUI_WIDGET_BUTTON :
		{
			ParserWidget = GuiButtonCreateDefault();
			break;
		}

		case GUI_WIDGET_TEXTBOX :
		{
			ParserWidget = GuiTextBoxCreateDefault();
			break;
		}


		case GUI_WIDGET_CONSOLE :
		{
			// ParserWidget = GuiConsoleCreateDefault();
			break;
		}

		case GUI_WIDGET_SCROLL :
		{
			// ParserWidget = GuiScrollCreateDefault();
			break;
		}

		case GUI_WIDGET_LABEL :
		{
			ParserWidget = GuiLabelCreateDefault();
			break;
		}

		case GUI_WIDGET_LIST :
		{
			ParserWidget = GuiListCreateDefault();
			break;
		}

		case GUI_WIDGET_IMAGE :
		{
			// ParserWidget = GuiImageCreateDefault();
			break;
		}
	}
}

static bool GuiParserEvent(char * Value)
{
	// Get the event pointer
	struct EventPtr * Event = GuiEventHandlerGet(Value);

	if (Event == NULL)
		return false;

	// Get the event type
	switch (GuiEventHandlerGetType(Value))
	{
		case GUI_EVENT_KEY_PRESSED :
		{
			ParserWidget->KeyPressed = Event->Ptr.KeyPressed;
			break;
		}

		case GUI_EVENT_KEY_RELEASED :
		{
			ParserWidget->KeyReleased = Event->Ptr.KeyReleased;
			break;
		}

		case GUI_EVENT_TEXT_ENTERED :
		{
			ParserWidget->TextEntered = Event->Ptr.TextEntered;
			break;
		}

		case GUI_EVENT_BUTTON_PRESSED :
		{
			ParserWidget->ButtonPressed = Event->Ptr.ButtonPressed;
			break;
		}

		case GUI_EVENT_BUTTON_RELEASED :
		{
			ParserWidget->ButtonReleased = Event->Ptr.ButtonReleased;
			break;
		}

		case GUI_EVENT_MOUSE_MOVE :
		{
			ParserWidget->MouseMove = Event->Ptr.MouseMove;
			break;
		}

		case GUI_EVENT_OPEN :
		{
			ParserWidget->Open = Event->Ptr.Open;
			break;
		}

		case GUI_EVENT_CLOSE :
		{
			ParserWidget->Close = Event->Ptr.Close;
			break;
		}
								
		case GUI_EVENT_ERROR :
			return false;

		default :
			return false;
	}

	return true;
}

static bool GuiParserValue(char * Value)
{
	switch (ParameterType)
	{
		case GUI_PARAMETER_TEXT :
		{
			if (strlen(Value) > 3)
			{
				char Temp[0xFF];

				strncpy(&Temp[0], &Value[0], 3);

				if (strncmp(&Temp[0], "MSG", 3) == 0)
				{
					sreadfield(Temp, Value, '_', 1);

					strcpy(ParserWidget->Text, LanguageMessageGet(Temp));

					break;
				}
				else if (strncmp(&Temp[0], "CFG", 3) == 0)
				{
					sreadfield(Temp, Value, '_', 1);

					strcpy(ParserWidget->Text, ConfigToString(Temp));

					break;
				}
			}

			strcpy(ParserWidget->Text, Value);
			break;
		}
						
		case GUI_PARAMETER_STYLE :
		{
			ParserWidget->Attributes.Form.Background = atoi(Value);
			break;
		}

		case GUI_PARAMETER_X :
		{
			ParserWidget->X = atoi(Value);
			break;
		}

		case GUI_PARAMETER_Y :
		{
			ParserWidget->Y = atoi(Value);
			break;
		}
	
		case GUI_PARAMETER_HEIGHT :
		{
			ParserWidget->Height = atoi(Value);
			break;
		}

		case GUI_PARAMETER_WIDTH :
		{
			ParserWidget->Width = atoi(Value);
			break;
		}

		case GUI_PARAMETER_FONTSIZE :
		{
			ParserWidget->FontSize = atoi(Value);
			break;
		}

		case GUI_PARAMETER_LENGTH :
		{
			ParserWidget->Length = atoi(Value);
			break;
		}

		case GUI_PARAMETER_HIDE :
		{
			ParserWidget->Attributes.TextBox.HideText = GetBool(Value);
			break;
		}

		case GUI_PARAMETER_EVENT :
		{
			if (!GuiParserEvent(Value))
				return false;
			break;
		}

		default :
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////
/// Parse each widget (return false on parsing error)
////////////////////////////////////////////////////////////
static bool GuiParserRead(struct Lexer * Input)
{
	while (ParserStatus != GUI_PARSER_STATUS_ERROR && ParserStatus != GUI_PARSER_STATUS_EOF)
	{
		// Skip the blank chars
		LexerSkip(Input, LEXER_BLANK);

		if (LexerOff(Input))
		{
			ParserStatus = GUI_PARSER_STATUS_EOF;
		}
		else
		{
			switch (ParserStatus)
			{
				case GUI_PARSER_STATUS_WAITING :
				{
					char Character;

					// Search for a next tag, comment or text
					LexerSkipTo(Input, GUI_PARSER_WIDGET_START GUI_PARSER_COMMENT LEXER_LOWER);

					// Get the value
					LexerNextChar(Input, GUI_PARSER_WIDGET_START GUI_PARSER_COMMENT LEXER_LOWER);

					if (!CheckIntegrity(Input))
						break;

					Character = LexerItemGet(Input)->data[0];

					bdestroy(LexerItemGet(Input));

					switch (Character)
					{
						case '<' :
						{
							ParserStatus = GUI_PARSER_STATUS_READING_WIDGET;
							break;
						}

						case ';' :
						{
							// Comment found, skip to the next line
							UInt32 Result = LexerNextLine(Input);

							ParserStatus = GUI_PARSER_STATUS_WAITING;

							if (Result == LEXER_EOF)
							{
								ParserStatus = GUI_PARSER_STATUS_EOF;
							}
							
							break;
						}

						default :
						{
							ParserStatus = GUI_PARSER_STATUS_ERROR;
							break;
						}
					}

					break;
				}

				case GUI_PARSER_STATUS_READING_WIDGET :
				{
					// Two possible cases, it's a simple widget or a closing tag
					LexerNextTo(Input, LEXER_BLANK GUI_PARSER_WIDGET_CLOSE);

					if (!CheckIntegrity(Input))
						break;

					if (LexerItemGet(Input)->data[0] == '/')
					{
						// Closing tag found
						WidgetType = GuiParserWidgetType(&LexerItemGet(Input)->data[1]);

						if (WidgetType == GUI_WIDGET_ERROR)
						{
							ParserStatus = GUI_PARSER_STATUS_ERROR;

							MessageError("GuiParserRead", "Invalid syntax in a widget type (closing) tag (character: %d)", Input->Current);
						}
						else
						{
							// Process it
							ParserStatus = GUI_PARSER_STATUS_PROCESSING;
							ProcessingStatus = GUI_PARSER_PROCESSING_CLOSING;
						}
					}
					else if (Lower(LexerItemGet(Input)->data, blength(LexerItemGet(Input))))
					{
						WidgetType = GuiParserWidgetType(LexerItemGet(Input)->data);

						if (WidgetType == GUI_WIDGET_ERROR)
						{
							ParserStatus = GUI_PARSER_STATUS_ERROR;

							MessageError("GuiParserRead", "Invalid widget type (character: %d)", Input->Current);
						}
						else
						{
							// Create a specific type widget
							GuiParserCreateWidget();

							// A simple widget, read it
							ParserStatus = GUI_PARSER_STATUS_READING_PARAMS;
						}
					}
					else
					{
						ParserStatus = GUI_PARSER_STATUS_ERROR;

						MessageError("GuiParserRead", "Invalid syntax in a widget type tag (character: %d)", Input->Current);
					}

					bdestroy(LexerItemGet(Input));

					break;
				}

				case GUI_PARSER_STATUS_READING_PARAMS :
				{
					char Character;
					
					// Go to the value
					LexerSkipTo(Input, LEXER_ALPHANUMERIC GUI_PARSER_WIDGET_FINISH GUI_PARSER_WIDGET_CLOSE);

					// It can be a param, a simple widget or an opening widget tag
					LexerNextTo(Input, GUI_PARSER_EQUAL GUI_PARSER_WIDGET_FINISH GUI_PARSER_WIDGET_CLOSE GUI_PARSER_COMMENT);

					if (LexerOff(Input))
					{
						ParserStatus = GUI_PARSER_STATUS_EOF;
						break;
					}
					else if (blength(LexerItemGet(Input)) == 0)
					{
						// Check the result
						LexerNextChar(Input, GUI_PARSER_EQUAL GUI_PARSER_WIDGET_FINISH GUI_PARSER_WIDGET_CLOSE GUI_PARSER_COMMENT);

						Character = LexerItemGet(Input)->data[0];

						bdestroy(LexerItemGet(Input));
						
						switch (Character)
						{
							case '/' :
							{
								// Process the widget (closing tag in the same line)
								ParserStatus = GUI_PARSER_STATUS_PROCESSING;
								ProcessingStatus = GUI_PARSER_PROCESSING_SIMPLE;
								break;
							}

							case '>' :
							{
								// Process the widget (opening tag)
								ParserStatus = GUI_PARSER_STATUS_PROCESSING;
								ProcessingStatus = GUI_PARSER_PROCESSING_OPENING;
								break;
							}

							case ';' :
							{
								// Comment found, skip to the next line
								UInt32 Status = LexerNextLine(Input);

								if (Status == LEXER_EOF)
								{
									ParserStatus = GUI_PARSER_STATUS_EOF;
								}
								
								break;
							}

							default :
							{
								ParserStatus = GUI_PARSER_STATUS_ERROR;
								break;
							}
						}
					}
					else
					{
						bstring Result = bstrcpy(LexerItemGet(Input));

						bdestroy(LexerItemGet(Input));

						// Check the result
						LexerNextChar(Input, GUI_PARSER_EQUAL GUI_PARSER_WIDGET_FINISH GUI_PARSER_COMMENT);

						Character = LexerItemGet(Input)->data[0];

						bdestroy(LexerItemGet(Input));
						
						switch (Character)
						{
							case '=' :
							{
								if (Lower(Result->data, blength(Result)))
								{
									ParameterType = GuiParserParamType(Result->data);
								}
								else
								{
									ParameterType = GUI_PARAMETER_ERROR;
								}

								break;
							}

							default :
							{
								ParserStatus = GUI_PARSER_STATUS_ERROR;
								break;
							}
						}

						bdestroy(Result);

						// Read the value
						if (ParameterType == GUI_PARAMETER_ERROR)
						{
							ParserStatus = GUI_PARSER_STATUS_ERROR;

							MessageError("GuiParserRead", "Invalid syntax in a widget parameter tag (character: %d)", Input->Current);
						}
						else
						{
							// Go to the value
							LexerSkipTo(Input, LEXER_ALPHANUMERIC);

							// Get the value
							LexerNextTo(Input, GUI_PARSER_VALUE_TAG);

							if (!CheckIntegrity(Input))
								break;

							// Add the value to the widget object
							if (!GuiParserValue(LexerItemGet(Input)->data))
							{
								ParserStatus = GUI_PARSER_STATUS_ERROR;

								MessageError("GuiParserRead", "Invalid parameter type (character: %d)", Input->Current);
							}

							bdestroy(LexerItemGet(Input));
						}
					}

					break;
				}

				case GUI_PARSER_STATUS_PROCESSING :
				{
					// Process the data
					switch (ProcessingStatus)
					{
						case GUI_PARSER_PROCESSING_SIMPLE :
						{
							// Create the window in the case of form
							if (WidgetType == GUI_WIDGET_FORM)
							{
								ParserWindow = GuiWindowCreate();

								if (ParserWidget->X == 16)
									ParserWidget->X = (WindowWidth / 2) - ((ParserWidget->Width + 16) / 2);
								
								if (ParserWidget->Y == 16)
									ParserWidget->Y = (WindowHeight / 2) - ((ParserWidget->Height + 16) / 2);
							}
							else if (WidgetType == GUI_WIDGET_TEXTBOX)
							{
								// Set the size
								if (ParserWidget->Length == 0)
									ParserWidget->Length = 30;

								if (ParserWidget->Width == 0)
									ParserWidget->Width = (ParserWidget->FontSize * ParserWidget->Length);

								if (ParserWidget->Height == 0)
									ParserWidget->Height = (ParserWidget->FontSize + 2);

								ParserWidget->Length = strlen(ParserWidget->Text);
								
								ParserWidget->Attributes.TextBox.CurrentPos = ParserWidget->Length;
							}
											
							// Add the widget
							GuiWindowAddWidget(ParserWindow, ParserWidget);
							
							break;
						}

						case GUI_PARSER_PROCESSING_CLOSING :
						{
							switch (WidgetType)
							{
								case GUI_WIDGET_FORM :
								{
									GuiLoadWindow(ParserWindow);
									break;
								}
							}
							break;
						}

						case GUI_PARSER_PROCESSING_OPENING :
						{
							switch (WidgetType)
							{
								case GUI_WIDGET_FORM :
								{
									// Create the window
									ParserWindow = GuiWindowCreate();

									if (ParserWidget->X == 16)
										ParserWidget->X = (WindowWidth / 2) - ((ParserWidget->Width + 16) / 2);
									
									if (ParserWidget->Y == 16)
										ParserWidget->Y = (WindowHeight / 2) - ((ParserWidget->Height + 16) / 2);
											
									// Add the widget
									GuiWindowAddWidget(ParserWindow, ParserWidget);
									break;
								}
							}

							break;
						}

						case GUI_PARSER_PROCESSING_ERROR :
						{
							ParserStatus = GUI_PARSER_STATUS_ERROR;
							break;
						}
					}

					// Wait for the next widget
					ParserStatus = GUI_PARSER_STATUS_WAITING;
					break;
				}

				default :
					break;
			}
		}
	}

	if (ParserStatus == GUI_PARSER_STATUS_ERROR)
		return false;

	return true;
}

////////////////////////////////////////////////////////////
/// Initialize the template and window functions
////////////////////////////////////////////////////////////
bool GuiParserInitialize(UInt32 DefaultTemplate)
{
	if (GuiParserLoadTemplate(DefaultTemplate))
		return GuiParserLoadWindows();

	return false;
}

////////////////////////////////////////////////////////////
/// Destroy all data
////////////////////////////////////////////////////////////
bool GuiParserDestroy()
{

	return true;
}

////////////////////////////////////////////////////////////
/// Load a template or change it if there are one
////////////////////////////////////////////////////////////
bool GuiParserLoadTemplate(UInt32 Index)
{

	return true;
}

////////////////////////////////////////////////////////////
/// Load all windows
////////////////////////////////////////////////////////////
bool GuiParserLoadWindows()
{
	bool Result = false;
	FILE * WindowFile;
	struct Lexer * Input;

	// Open the file
	if (!fopen(&WindowFile, GUI_PARSER_WINDOWS_PATH, "r"))
	{
		MessageError("GuiParserLoadWindows", "An internal user interface error : '%s' not found or corrupted.", GUI_PARSER_WINDOWS_PATH);
		return false;
	}

	// Initialize the lexer
	Input = LexerCreate(WindowFile);

	// Parse the input data
	Result = GuiParserRead(Input);	

	// Destroy the lexer
	LexerDestroy(Input);
	Input = NULL;

	// Close the file
	fclose(WindowFile);

	return Result;
}