////////////////////////////////////////////////////////////
///	@file    System/Parser.h
///	@author  N. Devillard
///	@date    Sep 2007
///	@version 3.0
///	@brief   Parser for ini files.
////////////////////////////////////////////////////////////

#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <System/Dictionary.h>

////////////////////////////////////////////////////////////
// Macros (for backwards compatibility only)
////////////////////////////////////////////////////////////
#define ParserGetStr(d, k)  ParserGetString(d, k, NULL)	
#define ParserSetStr        ParserSetString

////////////////////////////////////////////////////////////
///	@brief    Get number of sections in a dictionary
///	@param    d   Dictionary to examine
///	@return   int Number of sections found in dictionary
///	
///	This function returns the number of sections found in a dictionary.
///	The test to recognize sections is done on the string stored in the
///	dictionary: a section name is given as "section" whereas a key is
///	stored as "section:key", thus the test looks for entries that do not
///	contain a colon.
///	
///	This clearly fails in the case a section name contains a colon, but
///	this should simply be avoided.
///	
///	This function returns -1 in case of error.
////////////////////////////////////////////////////////////
int ParserGetNsec(struct Dictionary * d);

////////////////////////////////////////////////////////////
///	@brief    Get name for section n in a dictionary.
///	@param    d   Dictionary to examine
///	@param    n   Section number (from 0 to nsec-1).
///	@return   Pointer to char string
///	
///	This function locates the n-th section in a dictionary and returns
///	its name as a pointer to a string statically allocated inside the
///	dictionary. Do not free or modify the returned string!
///	
///	This function returns NULL in case of error.
////////////////////////////////////////////////////////////
char * ParserGetSecName(struct Dictionary * d, int n);

////////////////////////////////////////////////////////////
///	@brief    Save a dictionary to a loadable ini file
///	@param    d   Dictionary to dump
///	@param    f   Opened file pointer to dump to
///	@return   void
///	
///	This function dumps a given dictionary into a loadable ini file.
///	It is Ok to specify @c stderr or @c stdout as output files.
////////////////////////////////////////////////////////////
void ParserDumpIni(struct Dictionary * d, FILE * f);

////////////////////////////////////////////////////////////
///	@brief    Dump a dictionary to an opened file pointer.
///	@param    d   Dictionary to dump.
///	@param    f   Opened file pointer to dump to.
///	@return   void
///	
///	This function prints out the contents of a dictionary, one element by
///	line, onto the provided file pointer. It is OK to specify @c stderr
///	or @c stdout as output files. This function is meant for debugging
///	purposes mostly.
////////////////////////////////////////////////////////////
void ParserDump(struct Dictionary * d, FILE * f);

////////////////////////////////////////////////////////////
///	@brief    Get the string associated to a key
///	@param    d       Dictionary to search
///	@param    key     Key string to look for
///	@param    def     Default value to return if key not found.
///	@return   pointer to statically allocated character string
///	
///	This function queries a dictionary for a key. A key as read from an
///	ini file is given as "section:key". If the key cannot be found,
///	the pointer passed as 'def' is returned.
///	The returned char pointer is pointing to a string allocated in
/// the dictionary, do not free or modify it.
////////////////////////////////////////////////////////////
char * ParserGetString(struct Dictionary * d, const char * key, char * def);

////////////////////////////////////////////////////////////
///	@brief    Get the string associated to a key, convert to an int
///	@param    d Dictionary to search
///	@param    key Key string to look for
///	@param    notfound Value to return in case of error
///	@return   integer
///	
///	This function queries a dictionary for a key. A key as read from an
///	ini file is given as "section:key". If the key cannot be found,
///	the notfound value is returned.
///	
///	Supported values for integers include the usual C notation
///	so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
///	are supported. Examples:
///	
///	- "42"      ->  42
///	- "042"     ->  34 (octal -> decimal)
///	- "0x42"    ->  66 (hexa  -> decimal)
///	
///	Warning: the conversion may overflow in various ways. Conversion is
///	totally outsourced to strtol(), see the associated man page for overflow
///	handling.
///	
///	Credits: Thanks to A. Becker for suggesting strtol()
////////////////////////////////////////////////////////////
int ParserGetInt(struct Dictionary * d, const char * key, int notfound);

////////////////////////////////////////////////////////////
///	@brief    Get the string associated to a key, convert to a double
///	@param    d Dictionary to search
///	@param    key Key string to look for
///	@param    notfound Value to return in case of error
///	@return   double
///	
///	This function queries a dictionary for a key. A key as read from an
///	ini file is given as "section:key". If the key cannot be found,
///	the notfound value is returned.
////////////////////////////////////////////////////////////
double ParserGetDouble(struct Dictionary * d, char * key, double notfound);

////////////////////////////////////////////////////////////
///	@brief    Get the string associated to a key, convert to a boolean
///	@param    d Dictionary to search
///	@param    key Key string to look for
///	@param    notfound Value to return in case of error
///	@return   integer
///	
///	This function queries a dictionary for a key. A key as read from an
///	ini file is given as "section:key". If the key cannot be found,
///	the notfound value is returned.
///	
///	A true boolean is found if one of the following is matched:
///	
///	- A string starting with 'y'
///	- A string starting with 'Y'
///	- A string starting with 't'
///	- A string starting with 'T'
///	- A string starting with '1'
///	
///	A false boolean is found if one of the following is matched:
///	
///	- A string starting with 'n'
///	- A string starting with 'N'
///	- A string starting with 'f'
///	- A string starting with 'F'
///	- A string starting with '0'
///	
///	The notfound value returned if no boolean is identified, does not
///	necessarily have to be 0 or 1.
////////////////////////////////////////////////////////////
int ParserGetBoolean(struct Dictionary * d, const char * key, int notfound);

////////////////////////////////////////////////////////////
///	@brief    Set an entry in a dictionary.
///	@param    ini     Dictionary to modify.
///	@param    entry   Entry to modify (entry name)
///	@param    val     New value to associate to the entry.
///	@return   int 0 if Ok, -1 otherwise.
///	
///	If the given entry can be found in the dictionary, it is modified to
///	contain the provided value. If it cannot be found, -1 is returned.
///	It is Ok to set val to NULL.
////////////////////////////////////////////////////////////
int ParserSetString(struct Dictionary * ini, char * entry, char * val);

////////////////////////////////////////////////////////////
///	@brief    Delete an entry in a dictionary
///	@param    ini     Dictionary to modify
///	@param    entry   Entry to delete (entry name)
///	@return   void
///	
///	If the given entry can be found, it is deleted from the dictionary.
////////////////////////////////////////////////////////////
void ParserUnset(struct Dictionary * ini, char * entry);

////////////////////////////////////////////////////////////
///	@brief    Finds out if a given entry exists in a dictionary
///	@param    ini     Dictionary to search
///	@param    entry   Name of the entry to look for
///	@return   integer 1 if entry exists, 0 otherwise
///	
///	Finds out if a given entry exists in the dictionary. Since sections
///	are stored as keys with NULL associated values, this is the only way
///	of querying for the presence of sections in a dictionary.
////////////////////////////////////////////////////////////
int ParserFindEntry(struct Dictionary * ini, char * entry) ;

////////////////////////////////////////////////////////////
///	@brief    Parse an ini file and return an allocated dictionary object
///	@param    path Name of the ini file to read.
///	@return   Pointer to newly allocated dictionary
///	
///	This is the parser for ini files. This function is called, providing
///	the name of the file to be read. It returns a dictionary object that
///	should not be accessed directly, but through accessor functions
///	instead.
///	
///	The returned dictionary must be freed using ParserFreeDict().
////////////////////////////////////////////////////////////
struct Dictionary * ParserLoad(const char * path);

////////////////////////////////////////////////////////////
///	@brief    Free all memory associated to an ini dictionary
///	@param    d Dictionary to free
///	@return   void
///	
///	Free all memory associated to an ini dictionary.
///	It is mandatory to call this function before the dictionary object
///	gets out of the current context.
////////////////////////////////////////////////////////////
void ParserFreeDict(struct Dictionary * d);

#endif // SYSTEM_PARSER_H
