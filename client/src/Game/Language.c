////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Game/Language.h>
#include <Game/Configuration.h>
#include <System/IOHelper.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
char Messages[LANGUAGE_MESSAGE_SIZE][LANGUAGE_MESSAGE_LENGHT];

////////////////////////////////////////////////////////////
/// Lenguage initializer
////////////////////////////////////////////////////////////
bool LanguageInitialize()
{
	Int32 Idiom = ConfigGet()->Language;

	if (LanguageLoad(Idiom))
		return true;

	return false;
}

////////////////////////////////////////////////////////////
/// Message loader
////////////////////////////////////////////////////////////
bool LanguageLoad(Int32 Idiom)
{
	FILE * LangFile;
	char Path[0xFF];
	UInt32 i;

	switch (Idiom)
	{
		case LANGUAGE_SPANISH :
			strcpy(Path, "data/lang/esp.lng");
			break;

		case LANGUAGE_ENGLISH :
			strcpy(Path, "data/lang/eng.lng");
			break;

		default :
			MessageError("LanguageLoad", "An internal language error : The specified language doesn't exist.");
			return false;
	}

	if (!fopen(&LangFile, Path, "r"))
	{
		MessageError("LanguageLoad", "An internal language error : '%s' not found or corrupted.", Path);
		return false;
	}

	// Read all data
	for (i = 0; i < LANGUAGE_MESSAGE_SIZE; i++)
	{
		// Get the line
		fgets(&Messages[i][0], LANGUAGE_MESSAGE_LENGHT, LangFile);
		
		// Set the last char to null
		Messages[i][strlen(Messages[i]) - 1] = NULLCHAR;
	}

	fclose(LangFile);

	return true;
}

////////////////////////////////////////////////////////////
/// Message method
////////////////////////////////////////////////////////////
char * LanguageMessage(UInt32 MessageIndex)
{
	if (MessageIndex < LANGUAGE_MESSAGE_SIZE)
		return Messages[MessageIndex];

	return NULL;
}

////////////////////////////////////////////////////////////
/// Get the message id with a message text
////////////////////////////////////////////////////////////
UInt32 LanguageMessageText(char * MessageType)
{
	if		(strcmp(MessageType, "CONNECT") == 0)	return MSG_CONNECT;
	else if (strcmp(MessageType, "EXIT") == 0)		return MSG_EXIT;
	else if (strcmp(MessageType, "USER") == 0)		return MSG_USER;
	else if (strcmp(MessageType, "PASSWORD") == 0)	return MSG_PASSWORD;

	return LANGUAGE_MSG_ERROR;
}

////////////////////////////////////////////////////////////
/// Simplification method
////////////////////////////////////////////////////////////
char * LanguageMessageGet(char * Type)
{
	return LanguageMessage(LanguageMessageText(Type));
}