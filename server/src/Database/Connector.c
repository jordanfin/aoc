////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Database/Connector.h>

////////////////////////////////////////////////////////////
/// Create a connector
////////////////////////////////////////////////////////////
struct DataBase * ConnectorCreate(char * Server, char * User, char * Password, char * Database, UInt32 Port)
{
	struct DataBase * DB = (struct DataBase*)malloc(sizeof(struct DataBase));

	if (DB)
	{
		strcpy(DB->Server, Server);
		strcpy(DB->User, User);
		strcpy(DB->Password, Password);
		strcpy(DB->Database, Database);
		DB->Port = Port;

		return DB;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
/// Connect from MySQL database
////////////////////////////////////////////////////////////
bool ConnectorInitialize(struct DataBase * DB)
{
	if (DB)
	{
		DB->Connection = mysql_init(NULL);

		if (DB->Connection == NULL)
		{
			MessageError("Connector Error", "%d: %s\n", mysql_errno(DB->Connection), mysql_error(DB->Connection));
			ConnectorDestroy(DB);
			return DATABASE_ERROR;
		}

		if (mysql_real_connect(DB->Connection, DB->Server, DB->User, DB->Password, DB->Database, 0, NULL, 0) == NULL)
		{
			MessageError("Connector Error", "MySQL error %d: %s\n", mysql_errno(DB->Connection), mysql_error(DB->Connection));
			ConnectorDestroy(DB);
			return DATABASE_ERROR;
		}

		return DATABASE_SUCCESS;
	}

	return DATABASE_ERROR;
}

////////////////////////////////////////////////////////////
/// Disconnect from MySQL database
////////////////////////////////////////////////////////////
bool ConnectorDestroy(struct DataBase * DB)
{
	if (DB)
	{
		mysql_close(DB->Connection);

		return DATABASE_SUCCESS;
	}

	MessageError("Connector Error", "Error when server tried to destroy the connector - (%s / %s / %s / %d)",
				 DB->Server, DB->User, DB->Database, DB->Port);

	return DATABASE_ERROR;
}
