////////////////////////////////////////////////////////////
///	@file	System/Dictionary.c
///	@author	N. Devillard
///	@date	Sep 2007
///	@version	$Revision: 1.27 $
///	@brief	Implements a Dictionary for string variables.
///	
///	This module implements a simple Dictionary object, i.e. a list
///	of string/string associations. This object is useful to store e.g.
///	informations retrieved from a configuration file (ini files).
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/Dictionary.h>

////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////
#define MAXVALSZ			1024		///< Maximum value size for integers and doubles
#define DICTMINSZ			128			///< Minimal allocated number of entries in a Dictionary
#define DICT_INVALID_KEY    ((char*)-1)	///< Invalid key token

////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Doubles the allocated size associated to a pointer
/// 'size' is the current allocated size
////////////////////////////////////////////////////////////
static void * mem_double(void * ptr, int size)
{
    void * newptr;

    newptr = calloc(2*size, 1);

    if (newptr==NULL)
		return NULL;

    memcpy(newptr, ptr, size);
    free(ptr);

    return newptr;
}

////////////////////////////////////////////////////////////
///	@brief    Duplicate a string
///	@param    s String to duplicate
///	@return   Pointer to a newly allocated string, to be freed with free()
///	
///	This is a replacement for strdup(). This implementation is provided
///	for systems that do not have it.
////////////////////////////////////////////////////////////
static char * xstrdup(char * s)
{
    char * t;
    if (!s) return NULL;
    t = malloc(strlen(s)+1);
    if (t)
        strcpy(t,s);
    return t;
}

////////////////////////////////////////////////////////////
// Function codes
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
///	@brief	Compute the hash key for a string.
///	@param	key		Character string to use for key.
///	@return	1 unsigned int on at least 32 bits.
///	
///	This hash function has been taken from an Article in Dr Dobbs Journal.
///	This is normally a collision-free function, distributing keys evenly.
///	The key is stored anyway in the struct so that collision can be avoided
///	by comparing the key itself in last resort.
////////////////////////////////////////////////////////////
unsigned DictionaryHash(char * key)
{
	Int32 len, i;
	unsigned hash;

	len = strlen(key);

	for (hash = 0, i = 0; i < len; i++)
	{
		hash += (unsigned)key[i];
		hash += (hash<<10);
		hash ^= (hash>>6);
	}

	hash += (hash <<3);
	hash ^= (hash >>11);
	hash += (hash <<15);
	
	return hash;
}

////////////////////////////////////////////////////////////
///	@brief	Create a new Dictionary object.
///	@param	size	Optional initial size of the Dictionary.
///	@return	1 newly allocated Dictionary objet.
///	
///	This function allocates a new Dictionary object of given size and returns
///	it. If you do not know in advance (roughly) the number of entries in the
///	Dictionary, give size=0.
////////////////////////////////////////////////////////////
struct Dictionary * DictionaryNew(int size)
{
	struct Dictionary * d;

	// If no size was specified, allocate space for DICTMINSZ
	if (size < DICTMINSZ) size = DICTMINSZ;

	if (!(d = (struct Dictionary*)calloc(1, sizeof(struct Dictionary))))
		return NULL;

	d->size = size ;
	d->val  = (char**)calloc(size, sizeof(char*));
	d->key  = (char**)calloc(size, sizeof(char*));
	d->hash = (unsigned int*)calloc(size, sizeof(unsigned));

	return d;
}

////////////////////////////////////////////////////////////
///	@brief	Delete a Dictionary object
///	@param	d	Dictionary object to deallocate.
///	@return	void
///	
///	Deallocate a Dictionary object and all memory associated to it.
////////////////////////////////////////////////////////////
void DictionaryDel(struct Dictionary * d)
{
	Int32 i;

	if (d == NULL) return;

	for (i = 0; i < d->size; i++)
	{
		if (d->key[i]!=NULL)
			free(d->key[i]);

		if (d->val[i]!=NULL)
			free(d->val[i]);
	}

	free(d->val);
	free(d->key);
	free(d->hash);
	free(d);

	return;
}

////////////////////////////////////////////////////////////
///	@brief	Get a value from a Dictionary.
///	@param	d		Dictionary object to search.
///	@param	key		Key to look for in the Dictionary.
///	@param    def     Default value to return if key not found.
///	@return	1 pointer to internally allocated character string.
///	
///	This function locates a key in a Dictionary and returns a pointer to its
///	value, or the passed 'def' pointer if no such key can be found in
///	Dictionary. The returned character pointer points to data internal to the
///	Dictionary object, you should not try to free it or modify it.
////////////////////////////////////////////////////////////
char * DictionaryGet(struct Dictionary * d, char * key, char * def)
{
	unsigned	hash;
	Int32		i;

	hash = DictionaryHash(key);

	for (i = 0; i < d->size; i++)
	{
        if (d->key[i] == NULL)
            continue;

        // Compare hash
		if (hash == d->hash[i])
		{
            // Compare string, to avoid hash collisions
            if (!strcmp(key, d->key[i]))
				return d->val[i];
		}
	}

	return def;
}

////////////////////////////////////////////////////////////
///	@brief    Set a value in a Dictionary.
///	@param    d       Dictionary object to modify.
///	@param    key     Key to modify or add.
///	@param    val     Value to add.
///	@return   int     0 if Ok, anything else otherwise
///	
///	If the given key is found in the Dictionary, the associated value is
///	replaced by the provided one. If the key cannot be found in the
///	Dictionary, it is added to it.
///	
///	It is Ok to provide a NULL value for val, but NULL values for the Dictionary
///	or the key are considered as errors: the function will return immediately
///	in such a case.
///	
///	Notice that if you Dictionaryset a variable to NULL, a call to
///	Dictionaryget will return a NULL value: the variable will be found, and
///	its value (NULL) is returned. In other words, setting the variable
///	content to NULL is equivalent to deleting the variable from the
///	Dictionary. It is not possible (in this implementation) to have a key in
///	the Dictionary without value.
///	
///	This function returns non-zero in case of failure.
////////////////////////////////////////////////////////////
int DictionarySet(struct Dictionary * d, char * key, char * val)
{
	Int32		i;
	unsigned	hash;

	if (d == NULL || key == NULL) return -1;
	
	// Compute hash for this key
	hash = DictionaryHash(key);

	// Find if value is already in Dictionary
	if (d->n > 0)
	{
		for (i = 0; i < d->size; i++)
		{
            if (d->key[i] == NULL)
                continue;

			// Same hash value
			if (hash == d->hash[i])
			{
				// Same key
				if (!strcmp(key, d->key[i]))
				{
					// Found a value: modify and return
					if (d->val[i] != NULL)
						free(d->val[i]);

                    d->val[i] = val ? xstrdup(val) : NULL ;

                    // Value has been modified: return
					return 0;
				}
			}
		}
	}

	// Add a new value
	// See if Dictionary needs to grow
	if (d->n == d->size)
	{
		// Reached maximum size: reallocate Dictionary
		d->val  = (char**)mem_double(d->val, d->size * sizeof(char*));
		d->key  = (char**)mem_double(d->key, d->size * sizeof(char*));
		d->hash = (unsigned int*)mem_double(d->hash, d->size * sizeof(unsigned));
        
		// Cannot grow Dictionary
		if ((d->val == NULL) || (d->key == NULL) || (d->hash == NULL))
            return -1;

		// Double size
		d->size <<= 1;
	}

    // Insert key in the first empty slot
    for (i = 0; i < d->size; i++)
	{
		// Add key here
        if (d->key[i] == NULL)
            break;
    }

	// Copy key
	d->key[i]  = xstrdup(key);
    d->val[i]  = val ? xstrdup(val) : NULL;
	d->hash[i] = hash;
	d->n++;

	return 0;
}

////////////////////////////////////////////////////////////
///	@brief	Delete a key in a Dictionary
///	@param	d		Dictionary object to modify.
///	@param	key		Key to remove.
///	@return   void
///	
///	This function deletes a key in a Dictionary.
///	Nothing is done if the key cannot be found.
////////////////////////////////////////////////////////////
void DictionaryUnset(struct Dictionary * d, char * key)
{
	unsigned	hash;
	Int32		i;

	if (key == NULL)
		return;

	hash = DictionaryHash(key);

	for (i = 0; i < d->size; i++)
	{
        if (d->key[i] == NULL)
            continue;

        // Compare hash
		if (hash == d->hash[i])
		{
            // Compare string, to avoid hash collisions
            if (!strcmp(key, d->key[i]))
                break; // Found key
		}
	}

    if (i >= d->size)
        return;	// Key not found

    free(d->key[i]);
    d->key[i] = NULL;

    if (d->val[i] != NULL)
	{
        free(d->val[i]);
        d->val[i] = NULL;
    }

    d->hash[i] = 0;
    d->n--;
}

////////////////////////////////////////////////////////////
///	@brief	Dump a Dictionary to an opened file pointer.
///	@param	d	Dictionary to dump
///	@param	f	Opened file pointer.
///	@return	void
///	
///	Dumps a Dictionary onto an opened file pointer.
///	Key pairs are printed out as @c [Key]=[Value], one per line.
///	It is Ok to provide stdout or stderr as output file pointers.
////////////////////////////////////////////////////////////
void DictionaryDump(struct Dictionary * d, FILE * out)
{
	Int32 i;

	if ( (d == NULL) || (out == NULL) )
		return;
	
	if (d->n < 1)
	{
		fprintf(out, "empty Dictionary\n");
		return;
	}

	for (i = 0; i < d->size; i++)
	{
        if (d->key[i])
            fprintf(out, "%20s\t[%s]\n", d->key[i], d->val[i] ? d->val[i] : "UNDEF");
	}
}
