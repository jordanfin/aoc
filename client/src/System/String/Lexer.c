////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/String/Lexer.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define LEXER_BASE_ALLOC 16

////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Add the current char to the Item and reads the next
////////////////////////////////////////////////////////////
static void LexerAccept(struct Lexer * LexPtr)
{
	if (!LexerOff(LexPtr) && LexPtr->HasCurrent)
	{
		bconchar(LexPtr->Item, LexPtr->Current);

		// Read
		LexPtr->Current = fgetc(LexPtr->Input);
	}
}

////////////////////////////////////////////////////////////
/// Get a character from the input
////////////////////////////////////////////////////////////
static void LexerLookahead(struct Lexer * LexPtr)
{	
	if (LexPtr != NULL && !LexerOff(LexPtr) && !LexPtr->HasCurrent)
	{
		// Read
		LexPtr->Current = fgetc(LexPtr->Input);
		LexPtr->HasCurrent = true;
	}
}

////////////////////////////////////////////////////////////
/// Create a new item
////////////////////////////////////////////////////////////
static void LexerItemCreate(struct Lexer * LexPtr)
{
	if (LexPtr == NULL)
		return;
	
	if (LexPtr->OwnItem)
		btrunc(LexPtr->Item, 0);
	else
		// Do not destroy the item
		LexPtr->Item = bfromcstralloc(LEXER_BASE_ALLOC, "");

	LexPtr->OwnItem = true;
}

////////////////////////////////////////////////////////////
/// Lexer initializer (the FILE have to be opened)
////////////////////////////////////////////////////////////
struct Lexer * LexerCreate(FILE * File)
{
	struct Lexer * Result = NULL;

	if (File != NULL)
	{
		Result = (struct Lexer*)malloc(sizeof(struct Lexer));

		if (Result != NULL)
		{
			Result->Input		= File;
			Result->Current		= EOF;
			Result->HasCurrent	= false;
			Result->Item		= bfromcstralloc(LEXER_BASE_ALLOC, "");
			Result->OwnItem		= true;
			Result->Off			= false;
			Result->Started		= false;
		}
	}

	return Result;
}

////////////////////////////////////////////////////////////
/// Lexer destructor (it doesn't close the file)
////////////////////////////////////////////////////////////
void LexerDestroy(struct Lexer * LexPtr)
{
	if (LexPtr == NULL)
		return;

	if (LexPtr->OwnItem)
		bdestroy(LexPtr->Item);
	
	free(LexPtr);
}

////////////////////////////////////////////////////////////
/// Returns the last string read by the lexer
////////////////////////////////////////////////////////////
bstring LexerItemGet(struct Lexer * LexPtr)
{
	if (LexPtr != NULL && !LexerOff(LexPtr) && LexerStarted(LexPtr))
	{
		bstring Result = NULL;

		LexPtr->OwnItem = false;
		Result = LexPtr->Item;

		return Result;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
/// Check if the lexer began to read a file
////////////////////////////////////////////////////////////
bool LexerStarted(const struct Lexer * LexPtr)
{
	return (LexPtr && LexPtr->Started);
}

////////////////////////////////////////////////////////////
/// Check if the lexer finished to read a file
////////////////////////////////////////////////////////////
bool LexerOff(const struct Lexer * LexPtr)
{
	return (LexPtr && LexPtr->Off);
}

////////////////////////////////////////////////////////////
///	Reads as characters as possible belonging to Charset
////////////////////////////////////////////////////////////
void LexerNext(struct Lexer * LexPtr, const char * Charset)
{
	if (LexPtr != NULL && Charset != NULL)
	{
		LexerItemCreate(LexPtr);
		LexerLookahead(LexPtr);

		while (LexPtr->Current != EOF && strchr(Charset, LexPtr->Current))
		{
			LexerAccept(LexPtr);
		}

		LexPtr->Started = true;

		LexPtr->Off = (LexPtr->Current == EOF && blength(LexPtr->Item) == 0);
	}
}

////////////////////////////////////////////////////////////
///	Reads as many characters as possible not belonging to Charset
////////////////////////////////////////////////////////////
void LexerNextTo(struct Lexer * LexPtr, const char * Charset)
{
	if (LexPtr != NULL && Charset != NULL)
	{
		LexerItemCreate(LexPtr);
		LexerLookahead(LexPtr);

		while (LexPtr->Current != EOF && !strchr(Charset, LexPtr->Current))
		{
			LexerAccept(LexPtr);
		}

		LexPtr->Started = true;

		LexPtr->Off = (LexPtr->Current == EOF && blength(LexPtr->Item) == 0);
	}
}

////////////////////////////////////////////////////////////
///	Reads one character belonging to Charset
////////////////////////////////////////////////////////////
void LexerNextChar(struct Lexer * LexPtr, const char * Charset)
{
	if (LexPtr != NULL && Charset != NULL)
	{
		LexerItemCreate(LexPtr);
		LexerLookahead(LexPtr);

		if (LexPtr->Current != EOF && strchr(Charset, LexPtr->Current))
		{
			bconchar(LexPtr->Item, LexPtr->Current);
			LexPtr->HasCurrent = false;
		}

		LexPtr->Started = true;

		LexPtr->Off = (LexPtr->Current == EOF && blength(LexPtr->Item) == 0);
	}
}

////////////////////////////////////////////////////////////
/// Skip until the end of line
////////////////////////////////////////////////////////////
UInt32 LexerNextLine(struct Lexer * LexPtr)
{
	UInt32 Result = LEXER_EOF;

	// Skip separators
	if (!LexerOff(LexPtr))
	{
		LexerSkip(LexPtr, LEXER_SEPARATOR);
	}

	// Skip the garbage
	if (!LexerOff(LexPtr))
	{
		LexerNextTo(LexPtr, "\n");

		if (!LexerOff(LexPtr))
		{
			// Garbage found
			if (blength(LexerItemGet(LexPtr)) > 0)
			{
				Result = LEXER_ERROR;
			}

			bdestroy(LexerItemGet(LexPtr));
		}
	}

	if (!LexerOff(LexPtr))
	{
		LexerNextChar(LexPtr, "\n");

		if (!LexerOff(LexPtr))
		{
			if (blength(LexerItemGet(LexPtr)) > 0 && Result != LEXER_ERROR)
			{
				// '\n' found
				Result = LEXER_OK;
			}

			bdestroy(LexerItemGet(LexPtr));
		}
	}

	return Result;
}

////////////////////////////////////////////////////////////
///	Reads as many characters as possible belonging to
/// Charset (the read characters are lost)
////////////////////////////////////////////////////////////
void LexerSkip(struct Lexer * LexPtr, const char * Charset)
{
	LexerNext(LexPtr, Charset);
	LexerItemCreate(LexPtr);
}

////////////////////////////////////////////////////////////
///	Reads as many characters as possible not belonging to
/// Charset (the read characters are lost)
////////////////////////////////////////////////////////////
void LexerSkipTo(struct Lexer * LexPtr, const char * Charset)
{
	LexerNextTo(LexPtr, Charset);
	LexerItemCreate(LexPtr);
}
