////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Game/Configuration.h>
#include <Game/Language.h>
#include <System/IOHelper.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct Config AppConfig;			///< Application config

////////////////////////////////////////////////////////////
/// Basic error handler
////////////////////////////////////////////////////////////
Int32 ConfigErrorValue(UInt32 Index, char Result)
{
	MessageError("Config", "An internal error ocurred : ID(%d) VALUE(%c).", Index, Result);
	return CONFIG_ERROR;
}

////////////////////////////////////////////////////////////
/// Configuration initializer
////////////////////////////////////////////////////////////
void ConfigInitialize()
{
	FILE * File;

	if (fopen(&File, CONFIG_PATH, "r"))
	{
		// Get data
		fread(&AppConfig, sizeof(struct Config), 1, File);

		// Close it
		fclose(File);

		// Check the integrity
		if (ConfigCheckIntegrity(&AppConfig))
			return;
	}

	ConfigSetDefault(&AppConfig);

	// Save the default config
	ConfigSave();
}

////////////////////////////////////////////////////////////
/// Configuration default seter
////////////////////////////////////////////////////////////
void ConfigSetDefault(struct Config * Configuration)
{
	// Set all values as a default
	Configuration->Language = LANGUAGE_ENGLISH;
	Configuration->WindowStyle = Fullscreen;
	Configuration->BitsPerPixel = 32;
	Configuration->Vsync = true;
	Configuration->Name[0] = NULLCHAR;
	Configuration->Password[0] = NULLCHAR;
	Configuration->GuiTemplate = 0;
}

////////////////////////////////////////////////////////////
/// Configuration save data
////////////////////////////////////////////////////////////
bool ConfigSave()
{
	FILE * File;

	if (!fopen(&File, CONFIG_PATH, "w"))
	{
		MessageError("ConfigDestroy", "An internal error ocurred : '%s' not found or corrupted.", CONFIG_PATH);
		return false;
	}

	// Check if the config is correct
	if (!ConfigCheckIntegrity(&AppConfig))
		ConfigSetDefault(&AppConfig);

	// Put data
	fwrite(&AppConfig, sizeof(struct Config), 1, File);

	// Close it
	fclose(File);

	return true;
}

////////////////////////////////////////////////////////////
/// Check if the values are correct
////////////////////////////////////////////////////////////
Int32 ConfigCheckIntegrity(struct Config * Configuration)
{
	if (Configuration->Language > LANGUAGE_SIZE)
		return ConfigErrorValue(CONFIG_LANGUAGE, Configuration->Language);
	else if (Configuration->WindowStyle > Fullscreen)
		return ConfigErrorValue(CONFIG_WINDOW_MODE, Configuration->WindowStyle);
	else if (Configuration->BitsPerPixel > 32)
		return ConfigErrorValue(CONFIG_WINDOW_BPP, Configuration->BitsPerPixel);
	else if (Configuration->Vsync > 1)
		return ConfigErrorValue(CONFIG_WINDOW_VSYNC, Configuration->Vsync);

	return true;
}

////////////////////////////////////////////////////////////
/// Configuration getter
////////////////////////////////////////////////////////////
struct Config * ConfigGet()
{
	return &AppConfig;
}

////////////////////////////////////////////////////////////
/// Get values of config from an id
////////////////////////////////////////////////////////////
char * ConfigToString(char * ID)
{
	if		(strncmp(ID, "USER", 4) == 0)		return ConfigGet()->Name;
	else if (strncmp(ID, "PASSWORD", 8) == 0)	return ConfigGet()->Password;
	// else if (strncmp(ID, "LANG", 4) == 0)		return 
	// else if (strncmp(ID, "WSTYLE", 6) == 0)		return

	return NULL;
}