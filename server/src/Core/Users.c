////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Core/Users.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
// Mutex					UsersMutex;	///< User list mutex
struct UserListData		UserList;	///< User list

////////////////////////////////////////////////////////////
/// Initialize users data
////////////////////////////////////////////////////////////
bool UsersInitialize()
{
	// Initialize mutex
	// MutexInitialize(&UsersMutex);

	UserList.First = UserList.Last = NULL;

	return true;
}

////////////////////////////////////////////////////////////
/// Destroy all users
////////////////////////////////////////////////////////////
bool UsersDestroy()
{
	struct UserData * Iterator = UserList.First;

	// Remove all users
	while (Iterator != NULL)
	{
		struct UserData * TempUser = Iterator;
		
		Iterator = Iterator->Next;

		// Close the socket
		MutexLock(&TempUser->MutexData);
		
		SocketClose(&TempUser->Connection.Socket);

		MutexUnlock(&TempUser->MutexData);

		// Destroy the mutex
		MutexDestroy(&TempUser->MutexData);

		// Free it
		free(TempUser);

		TempUser = NULL;
	}

	return true;
}

////////////////////////////////////////////////////////////
/// Create a user
////////////////////////////////////////////////////////////
struct UserData * UserCreate()
{
	return (struct UserData*)malloc(sizeof(struct UserData));
}

////////////////////////////////////////////////////////////
/// Add user in the list
////////////////////////////////////////////////////////////
void UserAdd(struct UserData * User)
{
	// Initialize mutex
	MutexInitialize(&User->MutexData);

	// Set login status
	User->Logged = false;

	// Initialize account data
	AccountInitialize(User);

	// Set parser status
	User->Connection.Parser = PARSER_WAITING_PREAMBLE;

	// If list is empty, put the user at first
	if (UserList.First == NULL)
	{
		UserList.First = User;
		User->Previous = NULL;
	}
	else
	{
		UserList.Last->Next = User;
		User->Previous = UserList.Last;
	}

	if (UserList.Last != NULL)
	{
		struct UserData * UserPtr;

		MutexLock(&UserList.Last->MutexData);
		UserPtr = UserList.Last;
		UserList.Last = User;
		MutexUnlock(&UserPtr->MutexData);
	}
	else
	{
		UserList.Last = User;
	}

	User->Next = NULL;
}

////////////////////////////////////////////////////////////
/// Remove user from the list
////////////////////////////////////////////////////////////
void UserRemove(struct UserData * User)
{
	if (UserContains(User))
	{
		struct UserData * UserPtr;

		// Remove from the list
		MutexLock(&User->MutexData);

		// Update the previous node
		if (User->Previous == NULL)
		{
			MutexLock(&UserList.First->MutexData);
			UserPtr = UserList.First;
			UserList.First = User->Next;
		}
		else
		{
			MutexLock(&User->Previous->Next->MutexData);
			UserPtr = User->Previous->Next;
			User->Previous->Next = User->Next;
		}

		MutexUnlock(&UserPtr->MutexData);

		// Update the next node
		if (User->Next == NULL)
		{
			MutexLock(&UserList.Last->MutexData);
			UserPtr = UserList.Last;
			UserList.Last = User->Previous;
		}
		else
		{
			MutexLock(&User->Next->Previous->MutexData);
			UserPtr = User->Next->Previous;
			User->Next->Previous = User->Previous;
		}

		MutexUnlock(&UserPtr->MutexData);

		// Close the socket
		SocketClose(&User->Connection.Socket);

		// Unlock the mutex
		MutexUnlock(&User->MutexData);

		// Destroy the mutex
		MutexDestroy(&User->MutexData);

		// Delete it
		free(User);

		User = NULL;
	}
}

////////////////////////////////////////////////////////////
/// Check if the user is on the list
////////////////////////////////////////////////////////////
bool UserContains(struct UserData * User)
{
	if (User != NULL)
	{
		struct UserData * Iterator;

		Iterator = UserList.First;

		MutexLock(&User->MutexData);

		while (Iterator != NULL)
		{
			MutexLock(&Iterator->MutexData);

			if (Iterator == User)
			{
				MutexUnlock(&Iterator->MutexData);
				MutexUnlock(&User->MutexData);
				return true;
			}

			MutexUnlock(&Iterator->MutexData);

			Iterator = Iterator->Next;
		}
	
		MutexUnlock(&User->MutexData);
	}

	return false;
}

////////////////////////////////////////////////////////////
/// Wait the thread until the user isn't removed
////////////////////////////////////////////////////////////
void UserWaitUntilRemoved(struct UserData * User)
{
	while (UserContains(User))
	{
		MutexLock(&User->MutexData);
		
		ThreadSleep(&User->ThreadData, 10);

		MutexUnlock(&User->MutexData);
	}
}

////////////////////////////////////////////////////////////
/// User protocol parser
////////////////////////////////////////////////////////////
void UserParseData(void * User)
{
	// Socket incoming buffer
	Int8 Buffer[4096];

	// Socket incoming data size
	Int32 SizeRecived = 0;

	// User
	struct UserData * UserPtr = (struct UserData*)User;

	// Socket receive loop
	do
	{
		// Receive data
		if (SocketReceive(&UserPtr->Connection.Socket, Buffer, 4096, &SizeRecived) == Done)
		{
			// Lock until the user hasn't been processed
			MutexLock(&UserPtr->MutexData);

			if (!ProtocolParser(UserPtr, (UInt8*)Buffer, SizeRecived))
			{
				MutexUnlock(&UserPtr->MutexData);

				// Remove it
				UserRemove(UserPtr);

				return;
			}
		
			MutexUnlock(&UserPtr->MutexData);
		}

	} while (SizeRecived > 0);

	// Remove it
	UserRemove(UserPtr);
}

////////////////////////////////////////////////////////////
/// Connect the user
////////////////////////////////////////////////////////////
bool UserConnect(struct UserData * User)
{
	
	return true;
}