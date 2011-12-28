#ifndef GAME_CONFIGURATION_H
#define GAME_CONFIGURATION_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Window/Window.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define CONFIG_PATH				"data/config"
#define CONFIG_ERROR			-1
#define CONFIG_LANGUAGE			0x00
#define CONFIG_WINDOW_MODE		0x01
#define CONFIG_WINDOW_BPP		0x02
#define CONFIG_WINDOW_VSYNC		0x03

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Config
{
	UInt32 Language;			///< (Spanish, English)
	enum Style WindowStyle;		///< (NonStyle, Fullscreen)
	UInt32 BitsPerPixel;		///< (16, 24, 32)
	bool Vsync;					///< (enabled, disabled)
	char Name[0x40];			///< User name
	char Password[0x40];		///< User password
	UInt32 GuiTemplate;			///< Graphical user interface template
};

////////////////////////////////////////////////////////////
/// Configuration initializer
////////////////////////////////////////////////////////////
void ConfigInitialize();

////////////////////////////////////////////////////////////
/// Configuration default seter
////////////////////////////////////////////////////////////
void ConfigSetDefault(struct Config * Configuration);

////////////////////////////////////////////////////////////
/// Configuration save data
////////////////////////////////////////////////////////////
bool ConfigSave();

////////////////////////////////////////////////////////////
/// Check if the values are correct
////////////////////////////////////////////////////////////
Int32 ConfigCheckIntegrity(struct Config * Configuration);

////////////////////////////////////////////////////////////
/// Configuration getter
////////////////////////////////////////////////////////////
struct Config * ConfigGet();

////////////////////////////////////////////////////////////
/// Get values of config from an id
////////////////////////////////////////////////////////////
char * ConfigToString(char * ID);

#endif // GAME_CONFIGURATION_H