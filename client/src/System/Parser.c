////////////////////////////////////////////////////////////
///	@file    System/Parser.c
///	@author  N. Devillard
///	@date    Sep 2007
///	@version 3.0
///	@brief   Parser for ini files.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Parser.h>
#include <ctype.h>

#ifndef SYSTEM_WINDOWS
	#include <unistd.h>
#endif

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define ASCIILINESZ         (1024)
#define INI_INVALID_KEY     ((char*)-1)

////////////////////////////////////////////////////////////
// Internal data
////////////////////////////////////////////////////////////
enum LineStatus ///< This enum stores the status for each parsed line
{
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
};

////////////////////////////////////////////////////////////
///	@brief	Convert a string to lowercase.
///	@param	s	String to convert.
///	@return	ptr to statically allocated string.
///
///	This function returns a pointer to a statically allocated string
///	containing a lowercased version of the input string. Do not free
///	or modify the returned string! Since the returned string is statically
///	allocated, it will be modified at each function call (not re-entrant).
////////////////////////////////////////////////////////////
static char * strlwc(const char * s)
{
    static char l[ASCIILINESZ+1];
    int i;

    if (s == NULL)
		return NULL;

    memset(l, 0, ASCIILINESZ+1);
    i = 0;

	while (s[i] && i < ASCIILINESZ)
	{
        l[i] = (char)tolower((int)s[i]);
        i++;
    }

    l[ASCIILINESZ] = (char)0;

    return l;
}

////////////////////////////////////////////////////////////
///	@brief	Remove blanks at the beginning and the end of a string.
///	@param	s	String to parse.
///	@return	ptr to statically allocated string.
///
///	This function returns a pointer to a statically allocated string,
///	which is identical to the input string, except that all blank
///	characters at the end and the beg. of the string have been removed.
///	Do not free or modify the returned string! Since the returned string
///	is statically allocated, it will be modified at each function call
///	(not re-entrant).
////////////////////////////////////////////////////////////
static char * strstrip(char * s)
{
    static char l[ASCIILINESZ+1];
	char * last;

    if (s == NULL)
		return NULL;

	while (isspace((int)*s) && *s) s++;
	memset(l, 0, ASCIILINESZ+1);
	strcpy(l, s);
	last = l + strlen(l);

	while (last > l)
	{
		if ( !isspace((int)*(last-1)) )
			break;
		last--;
	}

	*last = (char)0;

	return (char*)l;
}

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
int ParserGetNsec(struct Dictionary * d)
{
    int i;
    int nsec;

    if (d == NULL)
		return -1;

	nsec = 0;

    for (i = 0; i < d->size; i++)
	{

		if (d->key[i] == NULL)
            continue;

        if (strchr(d->key[i], ':') == NULL)
            nsec++;
    }

    return nsec;
}

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
char * ParserGetSecName(struct Dictionary * d, int n)
{
    int i;
    int foundsec;

    if (d == NULL || n < 0)
		return NULL;

    foundsec = 0;

    for (i = 0; i < d->size; i++)
	{
        if (d->key[i] == NULL)
            continue;

        if (strchr(d->key[i], ':') == NULL)
		{
            foundsec++;

            if (foundsec > n)
                break;
        }
    }

    if (foundsec<=n)
        return NULL;

    return d->key[i];
}

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
void ParserDump(struct Dictionary * d, FILE * f)
{
    int i;

    if (d == NULL || f == NULL)
		return;

    for (i=0 ; i<d->size ; i++)
	{

		if (d->key[i] == NULL)
            continue;

        if (d->val[i]!=NULL)
            fprintf(f, "[%s]=[%s]\n", d->key[i], d->val[i]);
        else
            fprintf(f, "[%s]=UNDEF\n", d->key[i]);
    }
}

////////////////////////////////////////////////////////////
///	@brief    Save a dictionary to a loadable ini file
///	@param    d   Dictionary to dump
///	@param    f   Opened file pointer to dump to
///	@return   void
///
///	This function dumps a given dictionary into a loadable ini file.
///	It is Ok to specify @c stderr or @c stdout as output files.
////////////////////////////////////////////////////////////
void ParserDumpIni(struct Dictionary * d, FILE * f)
{
    int     i, j;
    char    keym[ASCIILINESZ+1];
    int     nsec;
    char *  secname;
    int     seclen;

    if (d == NULL || f == NULL)
		return;

    nsec = ParserGetNsec(d);

    if (nsec < 1)
	{
        // No section in file: dump all keys as they are
        for (i = 0; i < d->size; i++)
		{
            if (d->key[i] == NULL)
                continue;
            fprintf(f, "%s = %s\n", d->key[i], d->val[i]);
        }

        return;
    }

    for (i = 0 ; i < nsec; i++)
	{
        secname = ParserGetSecName(d, i);
        seclen  = (int)strlen(secname);
        fprintf(f, "\n[%s]\n", secname);
        sprintf(keym, "%s:", secname);

		for (j=0 ; j<d->size ; j++)
		{
            if (d->key[j] == NULL)
                continue;

            if (!strncmp(d->key[j], keym, seclen+1))
			{
                fprintf(f, "%-30s = %s\n", d->key[j]+seclen+1, d->val[j] ? d->val[j] : "");
            }
        }
    }

    fprintf(f, "\n");
}

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
///	the dictionary, do not free or modify it.
////////////////////////////////////////////////////////////
char * ParserGetString(struct Dictionary * d, const char * key, char * def)
{
    char * lc_key;
    char * sval;

    if (d == NULL || key == NULL)
        return def;

    lc_key = strlwc(key);
    sval = DictionaryGet(d, lc_key, def);

    return sval;
}

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
///	"42"      ->  42
///	"042"     ->  34 (octal -> decimal)
///	"0x42"    ->  66 (hexa  -> decimal)
///
///	Warning: the conversion may overflow in various ways. Conversion is
///	totally outsourced to strtol(), see the associated man page for overflow
///	handling.
///
///	Credits: Thanks to A. Becker for suggesting strtol()
////////////////////////////////////////////////////////////
int ParserGetInt(struct Dictionary * d, const char * key, int notfound)
{
    char * str;

    str = ParserGetString(d, key, INI_INVALID_KEY);

	if (str == INI_INVALID_KEY)
		return notfound;

    return (int)strtol(str, NULL, 0);
}

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
double ParserGetDouble(struct Dictionary * d, char * key, double notfound)
{
    char * str;

    str = ParserGetString(d, key, INI_INVALID_KEY);

    if (str==INI_INVALID_KEY)
		return notfound;

    return atof(str);
}

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
int ParserGetBoolean(struct Dictionary * d, const char * key, int notfound)
{
    char *c;
    int  ret;

    c = ParserGetString(d, key, INI_INVALID_KEY);

    if (c == INI_INVALID_KEY)
		return notfound;

	if (c[0]=='y' || c[0]=='Y' || c[0]=='1' || c[0]=='t' || c[0]=='T')
        ret = 1;
    else if (c[0]=='n' || c[0]=='N' || c[0]=='0' || c[0]=='f' || c[0]=='F')
        ret = 0;
	else
        ret = notfound;

    return ret;
}

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
int ParserFindEntry(struct Dictionary * ini, char * entry)
{
    int found = 0;

    if ( ParserGetString(ini, entry, INI_INVALID_KEY) != INI_INVALID_KEY)
        found = 1;

    return found;
}

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
int ParserSet(struct Dictionary * ini, char * entry, char * val)
{
    return DictionarySet(ini, strlwc(entry), val) ;
}

////////////////////////////////////////////////////////////
///	@brief    Delete an entry in a dictionary
///	@param    ini     Dictionary to modify
///	@param    entry   Entry to delete (entry name)
///	@return   void
///
///	If the given entry can be found, it is deleted from the dictionary.
////////////////////////////////////////////////////////////
void ParserUnset(struct Dictionary * ini, char * entry)
{
    DictionaryUnset(ini, strlwc(entry));
}

////////////////////////////////////////////////////////////
///	@brief	Load a single line from an INI file
///	@param    input_line  Input line, may be concatenated multi-line input
///	@param    section     Output space to store section
///	@param    key         Output space to store key
///	@param    value       Output space to store value
///	@return   line_status value
////////////////////////////////////////////////////////////
static enum LineStatus ParserLine(char * input_line, char * section, char * key, char * value)
{
    enum LineStatus sta;
    char        line[ASCIILINESZ+1];
    int         len;

    strcpy(line, strstrip(input_line));
    len = (int)strlen(line);

    sta = LINE_UNPROCESSED;

	if (len<1)
	{
        // Empty line
        sta = LINE_EMPTY;
    }
	else if (line[0]=='#')
	{
        // Comment line
        sta = LINE_COMMENT;
    }
	else if (line[0]=='[' && line[len-1]==']')
	{
        // Section name
        sscanf(line, "[%[^]]", section);
        strcpy(section, strstrip(section));
        strcpy(section, strlwc(section));
        sta = LINE_SECTION ;
    } else if (sscanf(line, "%[^=] = \"%[^\"]\"", key, value) == 2
           ||  sscanf(line, "%[^=] = '%[^\']'",   key, value) == 2
           ||  sscanf(line, "%[^=] = %[^;#]",     key, value) == 2)
	{

		// Usual key=value, with or without comments
        strcpy(key, strstrip(key));
        strcpy(key, strlwc(key));
        strcpy(value, strstrip(value));

        // sscanf cannot handle '' or "" as empty values this is done here
        if ( !strcmp(value, "\"\"") || (!strcmp(value, "''")) )
            value[0] = 0;

        sta = LINE_VALUE;
    }
	else if ( sscanf(line, "%[^=] = %[;#]", key, value ) == 2 ||  sscanf(line, "%[^=] %[=]", key, value) == 2)
	{
        // Special cases: key= || key=; || key=#
        strcpy(key, strstrip(key));
        strcpy(key, strlwc(key));
        value[0] = 0;
        sta = LINE_VALUE;
    } else
        sta = LINE_ERROR; // Generate syntax error

    return sta;
}

////////////////////////////////////////////////////////////
///	@brief    Parse an ini file and return an allocated dictionary object
///	@param    ininame Name of the ini file to read.
///	@return   Pointer to newly allocated dictionary
///
///	This is the parser for ini files. This function is called, providing
///	the name of the file to be read. It returns a dictionary object that
///	should not be accessed directly, but through accessor functions
///	instead.
///
///	The returned dictionary must be freed using ParserFreeDict().
////////////////////////////////////////////////////////////
struct Dictionary * ParserLoad(const char * ininame)
{
    FILE * in;

    char line    [ASCIILINESZ+1];
    char section [ASCIILINESZ+1];
    char key     [ASCIILINESZ+1];
    char tmp     [ASCIILINESZ+1];
    char val     [ASCIILINESZ+1];

    int  last = 0;
    int  len;
    int  lineno = 0;
    int  errs = 0;

    struct Dictionary * dict;

	fopen(&in, ininame, "r");

    if (in == NULL)
	{
        fprintf(stderr, "iniparser: cannot open %s\n", ininame);
        return NULL;
    }

    dict = DictionaryNew(0);

    if (!dict)
	{
        fclose(in);
        return NULL;
    }

    memset(line,    0, ASCIILINESZ);
    memset(section, 0, ASCIILINESZ);
    memset(key,     0, ASCIILINESZ);
    memset(val,     0, ASCIILINESZ);
    last = 0;

    while ( fgets(line+last, ASCIILINESZ-last, in) != NULL )
	{
        lineno++;
        len = (int)strlen(line)-1;
        // Safety check against buffer overflows
        if (line[len] != '\n')
		{
            fprintf(stderr, "iniparser: input line too long in %s (%d)\n", ininame, lineno);
            DictionaryDel(dict);
            fclose(in);
            return NULL;
        }

        // Get rid of \n and spaces at end of line
        while ( (len >= 0 ) && ( (line[len] == '\n') || (isspace(line[len])) ) )
		{
            line[len] = 0;
            len--;
        }

        // Detect multi-line
        if (line[len] == '\\')
		{
            last = len; // Multi-line value
            continue;
        }
		else
			last = 0;

        switch (ParserLine(line, section, key, val))
		{
            case LINE_EMPTY :
            case LINE_COMMENT :
				break;

            case LINE_SECTION :
				errs = DictionarySet(dict, section, NULL);
				break;

            case LINE_VALUE :
				sprintf(tmp, "%s:%s", section, key);
				errs = DictionarySet(dict, tmp, val);
				break;

            case LINE_ERROR:
				fprintf(stderr, "iniparser: syntax error in %s (%d):\n", ininame,  lineno);
				fprintf(stderr, "-> %s\n", line);
				errs++;
				break;

            default :  break;
        }

        memset(line, 0, ASCIILINESZ);
        last = 0;

        if (errs < 0)
		{
            fprintf(stderr, "iniparser: memory allocation failure\n");
            break;
        }
    }

    if (errs)
	{
        DictionaryDel(dict);
        dict = NULL;
    }

    fclose(in);

    return dict;
}

////////////////////////////////////////////////////////////
///	@brief    Free all memory associated to an ini dictionary
///	@param    d Dictionary to free
///	@return   void
///
///	Free all memory associated to an ini dictionary.
///	It is mandatory to call this function before the dictionary object
///	gets out of the current context.
////////////////////////////////////////////////////////////
void ParserFreeDict(struct Dictionary * d)
{
    DictionaryDel(d);
}
