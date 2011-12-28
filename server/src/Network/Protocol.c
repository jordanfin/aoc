////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Network/Protocol.h>
#include <Core/Users.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct SocketData	SocketListen;		///< Client socket struct
Mutex				ListenerMutex;		///< Socket listen mutex
Thread				ListenerThread;		///< Socket listen thread

////////////////////////////////////////////////////////////
// Packets
////////////////////////////////////////////////////////////
bool HandlePacketRequest(struct UserData * User);
bool HandlePacketLogin(struct UserData * User);
bool HandlePacketPlayer(struct UserData * User);

////////////////////////////////////////////////////////////
/// Protocol initializer
////////////////////////////////////////////////////////////
bool ProtocolInitialize(UInt16 Port)
{
	// Initialize socket helper from Win32
	SocketHelperInitialize();

	// Initialize Socket mutex
	MutexInitialize(&ListenerMutex);

	if (!SocketListenPort(&SocketListen, Port))
		return false;

	// Launch the thread
	ThreadLaunch(&ListenerThread, (FuncType)&ProtocolListener, NULL);

	return true;
}

////////////////////////////////////////////////////////////
/// Protocol destroyer
////////////////////////////////////////////////////////////
bool ProtocolDestroy()
{
	// Close socket
	if (!SocketClose(&SocketListen))
		return false;

	// Destroy socket helper
	SocketHelperCleanup();
	
	// Destroy mutex
	MutexDestroy(&ListenerMutex);

	return true;
}

////////////////////////////////////////////////////////////
/// Socket listener proc
////////////////////////////////////////////////////////////
void ProtocolListener()
{
	do
	{
		struct SocketData Socket;

		if (SocketAccept(&SocketListen, &Socket) == Done)
		{
			struct UserData * User;

			// Create it
			User = UserCreate();

			memcpy(&User->Connection.Socket, &Socket, sizeof(struct SocketData));
			
			UserAdd(User);

			// Launch thread
			ThreadLaunch(&User->ThreadData, (FuncType)&UserParseData, (void*)User);
		}
	} while (SocketIsValid(&SocketListen));
}

////////////////////////////////////////////////////////////
/// Protocol parser
////////////////////////////////////////////////////////////
bool ProtocolParser(struct UserData * User, UInt8 * Data, Int32 DataSize)
{
	Int32 i; UInt8 ByteData;

	for (i = 0; i < DataSize; i++)
	{
		ByteData = Data[i];

		switch (User->Connection.Parser)
		{
			case PARSER_WAITING_PREAMBLE :
				if (ByteData ==  NETWORK_PREAMBLE)
					User->Connection.Parser = PARSER_WAITING_TYPE;
				break;

			case PARSER_WAITING_TYPE :
				User->Connection.Type = ByteData;
				User->Connection.Parser = PARSER_WAITING_CODE;
				break;

			case PARSER_WAITING_CODE :
				User->Connection.Code = ByteData;
				User->Connection.Parser = PARSER_WAITING_LENGTH;
				break;

			case PARSER_WAITING_LENGTH :
				User->Connection.PacketLength = ByteData;
				User->Connection.Parser = PARSER_WAITING_CLENGTH;
				break;

			case PARSER_WAITING_CLENGTH :

				if (User->Connection.PacketLength + ByteData != 255)
				{
					User->Connection.Parser = PARSER_WAITING_PREAMBLE;
	                break;
				}

				if (ByteData == 0)
				{
					User->Connection.Parser = PARSER_WAITING_PREAMBLE;
					return ProtocolHandleUser(User);
				}

				if (ByteData > 0 )
				{
					User->Connection.CurrentLength = 0;
					User->Connection.Parser = PARSER_WAITING_DATA;
				}
				break;

			case PARSER_WAITING_DATA :
				User->Connection.Packet.Buffer[User->Connection.CurrentLength++] = ByteData;

				if (User->Connection.CurrentLength == User->Connection.PacketLength)
				{
					User->Connection.Parser = PARSER_WAITING_PREAMBLE;

					// Return false if the data is wrong
					return ProtocolHandleUser(User);
				}
				break;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////
/// Handles incoming data socket
////////////////////////////////////////////////////////////
bool ProtocolHandleUser(struct UserData * User)
{
	if (User->Logged == false)
	{
		switch (User->Connection.Type)
		{
			case NETWORK_TYPE_LOGIN :
				switch (User->Connection.Code)
				{
					case NETWORK_CODE_REQUEST :
						return HandlePacketRequest(User);

					case NETWORK_CODE_LOGIN :
						return HandlePacketLogin(User);

					case NETWORK_CODE_PLAYER :
						return HandlePacketPlayer(User);
				}

			default : // If packet type is another else...
				return false;
		}
	}
	else
	{
		switch (User->Connection.Type)
		{
			// More packets..
		}
	}

    return false;
}

////////////////////////////////////////////////////////////
/// Protocol send error
////////////////////////////////////////////////////////////
void ProtocolSendError(struct UserData * User, UInt32 Code)
{
	struct PacketError Error;

	Error.Code = Code;

	ProtocolSendData(User, NETWORK_TYPE_ERROR, NETWORK_CODE_ERROR, &Error, sizeof(struct PacketError));
}

////////////////////////////////////////////////////////////
/// Protocol send data procediment
////////////////////////////////////////////////////////////
void ProtocolSendData(struct UserData * User, UInt8 Type, UInt8 Code, void * Data, Int8 Size)
{
	// Buffer data
	Int8 Buffer[255];

	if (!SocketIsValid(&User->Connection.Socket)) return;

	// Set buffer initial data
	Buffer[0] = NETWORK_PREAMBLE;
	Buffer[1] = Type;
	Buffer[2] = Code;
	Buffer[3] = Size;
	Buffer[4] = ~Size;

	// Put data into buffer
	if ( Size > 0 )
	{
		memcpy(&Buffer[5], Data, Size);
	}

	// Send data to Server
	SocketSend(&User->Connection.Socket, &Buffer[0], Size + 5);
}

////////////////////////////////////////////////////////////
/// Protocol packet request handler
////////////////////////////////////////////////////////////
bool HandlePacketRequest(struct UserData * User)
{
	if (User->Connection.PacketLength == sizeof(struct PacketRequest))
	{
		struct PacketBool Boolean;

		#ifndef DEBUG
			char MD5Code[16] = "0000000000000000";
		#else
			char MD5Code[16] = "abcdefghijklmnop";
		#endif
		
		Boolean.Value = ( (strcmp(User->Connection.Packet.Request.MD5, MD5Code) != 0) ||
						  (User->Connection.Packet.Request.AppMajor != APP_MAJOR_VERSION) ||
						  (User->Connection.Packet.Request.AppMinor != APP_MINOR_VERSION) ||
						  (User->Connection.Packet.Request.AppRevision != APP_REVISION_VERSION) );

		// Send to the user
		ProtocolSendData(User, NETWORK_TYPE_LOGIN, NETWORK_CODE_RESULT, &Boolean, sizeof(struct PacketBool));
		
		return Boolean.Value;
	}

	return false;
}

////////////////////////////////////////////////////////////
/// Protocol packet login handler
////////////////////////////////////////////////////////////
bool HandlePacketLogin(struct UserData * User)
{
	if (User->Connection.PacketLength == sizeof(struct PacketLogin))
	{
		// If the user is logged
		if (User->Logged)
		{
			// Send the error
			ProtocolSendError(User, NETWORK_ERROR_USR_LOG);
			return false;
		}

		// Get the data
		strcpy(User->Name, User->Connection.Packet.Login.User);
		strcpy(User->Password, User->Connection.Packet.Login.Password);

		// Login
		return AccountLogin(DataBaseGetDefault(), User);
	}
						
	return false;
}

////////////////////////////////////////////////////////////
/// Protocol packet player handler
////////////////////////////////////////////////////////////
bool HandlePacketPlayer(struct UserData * User)
{
	if (User->Connection.PacketLength == sizeof(struct PacketPlayer))
	{
		// If the user is logged
		if (User->Logged)
		{
			// Send the error
			ProtocolSendError(User, NETWORK_ERROR_USR_LOG);
			return false;
		}

		// Login the player
		return UserConnect(User);
	}

	return false;
}