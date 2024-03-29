////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Database/Default.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct DataBase * DataBaseDefault;

/////////////////////////////////////////////////////////////
/// Initialize a default database
/////////////////////////////////////////////////////////////
bool DataBaseInitialize()
{
	// Create it
	DataBaseDefault = ConnectorCreate(DATABASE_DEFAULT_SERVER,
									  DATABASE_DEFAULT_USER,
									  DATABASE_DEFAULT_PASSWORD,
									  DATABASE_DEFAULT_NAME,
									  DATABASE_DEFAULT_PORT);

	if (DataBaseDefault)
		return ConnectorInitialize(DataBaseDefault);

	return false;
}

/////////////////////////////////////////////////////////////
/// Destroy a default database
/////////////////////////////////////////////////////////////
bool DataBaseDestroy()
{
	return ConnectorDestroy(DataBaseDefault);
}

/////////////////////////////////////////////////////////////
/// Get default database
/////////////////////////////////////////////////////////////
struct DataBase * DataBaseGetDefault()
{
	return DataBaseDefault;
}
