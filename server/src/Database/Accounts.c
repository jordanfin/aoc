////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Database/Accounts.h>

////////////////////////////////////////////////////////////
///	Initialize account data
////////////////////////////////////////////////////////////
void AccountInitialize(struct UserData * User)
{
	UInt32 i;

	User->Name[0] = User->Password[0] = NULLCHAR;

	for (i = 0; i < ACCOUNT_PLAYERS_SIZE; i++)
		User->Players[i][0] = NULLCHAR;
}

////////////////////////////////////////////////////////////
///	Check if the account exist in the database
////////////////////////////////////////////////////////////
bool AccountExist(struct DataBase * DB, struct UserData * User)
{
	return (Query(DB, "SELECT id FROM `%s` WHERE name='%s'", ACCOUNT_TABLE_DEFAULT, User->Name) != NULL);
}

////////////////////////////////////////////////////////////
///	Check if the password is correct
////////////////////////////////////////////////////////////
bool AccountCheckPassword(struct DataBase * DB, struct UserData * User)
{
	return (Query(DB, "SELECT password FROM `%s` WHERE name='%s' AND password='%s'",
			   ACCOUNT_TABLE_DEFAULT, User->Name, User->Password) != NULL);
}

////////////////////////////////////////////////////////////
///	Check if the email is correct
////////////////////////////////////////////////////////////
bool AccountCheckEmail(struct DataBase * DB, struct UserData * User, char * Email)
{
	return (Query(DB, "SELECT email FROM `%s` WHERE name='%s' AND email='%s'",
			   ACCOUNT_TABLE_DEFAULT, User->Name, Email) != NULL);
}

////////////////////////////////////////////////////////////
///	Check if the account is banned
////////////////////////////////////////////////////////////
bool AccountCheckBan(struct DataBase * DB, struct UserData * User)
{
	return (Query(DB, "SELECT ban FROM `%s` WHERE name='%s'",
			   ACCOUNT_TABLE_DEFAULT, User->Name) != NULL);
}

////////////////////////////////////////////////////////////
/// Ban an account
////////////////////////////////////////////////////////////
bool AccountSetBan(struct DataBase * DB, struct UserData * User, bool Ban)
{
	return (Query(DB, "UPDATE `%s` SET ban='%c' WHERE name='%s'",
			   ACCOUNT_TABLE_DEFAULT, Ban, User->Name) != NULL);
}

////////////////////////////////////////////////////////////
///	Login an account
////////////////////////////////////////////////////////////
bool AccountLogin(struct DataBase * DB, struct UserData * User)
{
	// Check the password
	if (AccountCheckPassword(DB, User))
	{
		// Check the ban
		if (AccountCheckBan(DB, User))
		{
			UInt32 i, Lenght = 0;
			char PlayersBuffer[0xC6];

			// Get accounts
			for (i = 0; i < ACCOUNT_PLAYERS_SIZE; i++)
			{
				UInt32 PlayerLenght;

				// Execute the query and get the data
				QueryGet(DB, User->Players[i], Query(DB, "SELECT `player%d` FROM `%s` WHERE name='%s'", i, ACCOUNT_TABLE_DEFAULT, User->Name));
				
				// Copy to the buffer
				strcpy(&PlayersBuffer[Lenght], User->Players[i]);

				// Get the player lenght
				PlayerLenght = strlen(User->Players[i]);

				// Add to the total lenght
				Lenght += PlayerLenght;

				// Check if we need a separator
				if (PlayerLenght)
					PlayersBuffer[Lenght++] = ';';
			}

			User->Logged = true;

			// Send packet
			ProtocolSendData(User, NETWORK_TYPE_LOGIN, NETWORK_CODE_ACCOUNT, &PlayersBuffer, Lenght);

			return true;
		}
		else
		{
			// The account is banned
			ProtocolSendError(User, NETWORK_ERROR_ACC_BAN);
		}

	}
	else
	{
		// The password is incorrect
		ProtocolSendError(User, NETWORK_ERROR_ACC_PASS);
	}

	User->Logged = false;

	return false;
}

////////////////////////////////////////////////////////////
///	Save an account
////////////////////////////////////////////////////////////
bool AccountExit(struct DataBase * DB, struct UserData * User)
{
	User->Logged = false;

	// Check the password (for integrity)
	if (AccountCheckPassword(DB, User))
	{
		// Save the user ip
		// ..

		// Send packet
		// ProtocolSendData(

		return true;
	}

	// Ban this account!
	AccountSetBan(DB, User, true);

	// Send the error
	// ProtocolSendError(User, PROTOCOL_ERROR_ACC_EXIT);

	return false;
}
