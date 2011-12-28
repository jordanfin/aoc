////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Database/Query.h>

/////////////////////////////////////////////////////////////
/// Makes a query to the database
/////////////////////////////////////////////////////////////
MYSQL_RES * Query(struct DataBase * DB, char * Query, ...)
{
	// Check if the database is avaible
	if (DB && DB->Connection)
	{
		char QueryFormated[0xFF];

		// Argument list
		va_list ArgList;

		// Initialize the argument list
		va_start(ArgList, Query);

		// sprintf arguments on string
		#ifdef SYSTEM_WINDOWS
			vsprintf_s(QueryFormated, 0xFF, Query, ArgList);
		#else
			vsprintf(QueryFormated, Query, ArgList);
		#endif

		// Destroy argument list
		va_end(ArgList);
		
		// Check if the connection is active
		if (mysql_ping(DB->Connection))
		{
			// Show error
			MessageError("Query",
						 "Error: The program couldn't make a query. %s. Trying to reconnect", mysql_error(DB->Connection));
			// Close it
			mysql_close(DB->Connection);

			// Reconnect
			ConnectorInitialize(DB);

			return NULL;
		}

		// Make the query
		if (!mysql_query(DB->Connection, QueryFormated))
		{
			MYSQL_RES * Result = mysql_store_result(DB->Connection);

			if ( Result != NULL )
			{
				if (mysql_num_rows(Result))
					return Result;
			}
		}
	}
	
	// Show error
	MessageError("Query", "Error: %s", mysql_error(DB->Connection));

	// Close it
	mysql_close(DB->Connection);

	return NULL;
}

/////////////////////////////////////////////////////////////
/// Process the query of database and returns it
/////////////////////////////////////////////////////////////
bool QueryGet(struct DataBase * DB, char * QueryResult, MYSQL_RES * Result)
{
	// Check if the database is avaible
	if (DB && DB->Connection && Result)
	{
		// Get the row
		MYSQL_ROW Row = mysql_fetch_row(Result);

		if (Row[0] == NULL)
			QueryResult = NULL;
		else
		{
			// Copy the result
			strcpy(QueryResult, (char *)Row[0]);
		}

		// Free it
		mysql_free_result(Result);

		return true;
	}

	// Show the error
	MessageError("QueryGet", "Error: Invalid Result");

	return false;
}
