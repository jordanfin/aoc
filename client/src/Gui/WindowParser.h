#ifndef GUI_WINDOW_PARSER_H
#define GUI_WINDOW_PARSER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

///////////////////////////////////////////////////////////////////////////
// DOC: This is the user interface parser, used for read and
//		load the forms from a script ubicated in a some path.
//
//		 e.g.:
//			<form name="frmMain" width="300" height="300" onload="LoadMain">
//				<textbox length="30" text="hello world" />
//				<image width="200 height=300 />
//				<button ... />
//			</form>
//
//      By default :
//          text = ""
//          fontsize = 8
//          x = centred
//          y = centred
//
//      Note :
//			- The parser is case sensitive
//          - Do not use the characters '^', '|', these are reserved for the parser
//          - Remember, use the character ';' to create comments
//          - The function id has to be always like: "(Window)(Widget)_(EventType)"
//              e.g.: "AccConnect_ButtonPressed"
//
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define GUI_EVENTS_SIZE                 0xFF
#define GUI_PARSER_WINDOWS_PATH         "data/gui/windows.ui"
#define GUI_PARSER_TEMPLATES_PATH       "data/gui/template%d.ui"

#define GUI_PARSER_WIDGET_START			"<"
#define GUI_PARSER_WIDGET_FINISH		"/"
#define GUI_PARSER_WIDGET_CLOSE			">"
#define GUI_PARSER_COMMENT              ";"
#define GUI_PARSER_EQUAL				"="
#define GUI_PARSER_VALUE_TAG			"\""
#define GUI_PARSER_VALUE_EVENT			"_"

#define GUI_PARAMETER_ERROR				0x00
#define GUI_PARAMETER_TEXT				0x01
#define GUI_PARAMETER_STYLE				0x02
#define GUI_PARAMETER_X					0x03
#define GUI_PARAMETER_Y					0x04
#define GUI_PARAMETER_HEIGHT			0x05
#define GUI_PARAMETER_WIDTH				0x06
#define GUI_PARAMETER_FONTSIZE			0x07
#define GUI_PARAMETER_LENGTH			0x08
#define GUI_PARAMETER_HIDE				0x09
#define GUI_PARAMETER_EVENT				0x0A

////////////////////////////////////////////////////////////
/// Initialize the template and window functions
////////////////////////////////////////////////////////////
bool GuiParserInitialize(UInt32 DefaultTemplate);

////////////////////////////////////////////////////////////
/// Destroy all data
////////////////////////////////////////////////////////////
bool GuiParserDestroy();

////////////////////////////////////////////////////////////
/// Load a template or change it if there are one
////////////////////////////////////////////////////////////
bool GuiParserLoadTemplate(UInt32 Index);

////////////////////////////////////////////////////////////
/// Load all windows
////////////////////////////////////////////////////////////
bool GuiParserLoadWindows();

#endif // GUI_WINDOWPARSER_H