////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Network/Protocol.h>
#include <Network/Socket.h>

#include <System/Thread.h>
#include <System/Mutex.h>
#include <System/IOHelper.h>
#include <System/Error.h>

#include <Game/User.h>
#include <Game/Login.h>
#include <Game/Application.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
UInt32				ConnectionStatus = NETWORK_DESTROYED;	///< Connection status controller
Mutex				ConnectionMutex;						///< Mutex for the connection controller
struct SocketData	SocketClient;							///< Client socket struct
Thread				SocketThread;							///< Protocol thread proc
Mutex				SocketMutex;							///< Mutex for the socket proc

////////////////////////////////////////////////////////////
/// Packets
////////////////////////////////////////////////////////////
bool HandlePacketResult();
bool HandlePacketAccount();

////////////////////////////////////////////////////////////
/// Create connection
////////////////////////////////////////////////////////////
void ProtocolInitialize()
{
	// Initialize the socket mutex
	MutexInitialize(&SocketMutex);

	// Initialize the connection mutex
	MutexInitialize(&ConnectionMutex);

	// Initialize socket helper from Win32
	SocketHelperInitialize();
}

////////////////////////////////////////////////////////////
/// Destroy connection
////////////////////////////////////////////////////////////
void ProtocolDestroy()
{
	// Destroy the scoket mutex
	MutexDestroy(&SocketMutex);

	// Destroy the connection mutex
	MutexDestroy(&ConnectionMutex);

	// Clean the socket helper from Win32
	SocketHelperCleanup();
}

////////////////////////////////////////////////////////////
/// Get the client socket
////////////////////////////////////////////////////////////
void ProtocolGetSocket(struct SocketData * Copy)
{
	MutexLock(&SocketMutex);

	memcpy(Copy, &SocketClient, sizeof(struct SocketData));

	MutexUnlock(&SocketMutex);
}

////////////////////////////////////////////////////////////
/// Get the client connection status
////////////////////////////////////////////////////////////
UInt32 ProtocolGetStatus()
{
	UInt32 Result;

	MutexLock(&ConnectionMutex);

	Result = ConnectionStatus;

	MutexUnlock(&ConnectionMutex);

	return Result;
}

////////////////////////////////////////////////////////////
/// Set the client connection status
////////////////////////////////////////////////////////////
void ProtocolSetStatus(UInt32 Status)
{
	MutexLock(&ConnectionMutex);

	ConnectionStatus = Status;

	MutexUnlock(&ConnectionMutex);
}

////////////////////////////////////////////////////////////
/// Connect to the server
////////////////////////////////////////////////////////////
void ProtocolConnect()
{
	if (ProtocolGetStatus() != NETWORK_CONNECTED)
	{
		struct SocketData Socket;

		ProtocolGetSocket(&Socket);

		// Try to connect
		if (SocketConnect(&Socket, NETWORK_CONNECTION_PORT, NETWORK_CONNECTION_IP, 3.0f))
		{
			// Initialize user
			UserReset();

			// Any more thing to reset?
			// ...

			// Connected
			ProtocolSetStatus(NETWORK_CONNECTED);

			// Launch the thread
			ThreadLaunch(&SocketThread, (FuncType)&ProtocolListen, NULL);

			// Send packet request to server
			PacketSendRequest();
		}
		else
		{
			// Disconnect from the server
			SocketClose(&Socket);

			// Show the message error
			MessageError("ProtocolConnect", "An error ocurred when the client was trying to connect to the server.");
		}
	}
}

////////////////////////////////////////////////////////////
/// Disconnect from the server
////////////////////////////////////////////////////////////
bool ProtocolDisconnect()
{
	// TODO: This is not clear..
	if (ProtocolGetStatus() == NETWORK_CONNECTED)
	{
		struct SocketData Socket;

		ProtocolGetSocket(&Socket);

		return (SocketClose(&Socket) == Done); // Wait until the thread finishes?
	}

	return true;
}

////////////////////////////////////////////////////////////
/// Listen procediment
////////////////////////////////////////////////////////////
void ProtocolListen()
{
	while (ProtocolGetStatus() == NETWORK_CONNECTED)
	{
		// Socket incoming buffer
		Int8 Buffer[4096];

		// Socket incoming data size
		Int32 SizeRecived = 0;

		// Client socket copy
		struct SocketData Socket;

		// Check the result of the socket
		enum SocketStatus Result;

		// Get the socket
		ProtocolGetSocket(&Socket);

		// Recive data
		Result = SocketReceive(&Socket, Buffer, 4096, &SizeRecived);

		// Handle the result
		switch (Result)
		{
			case Done :
				// Parse the data
				ProtocolParser(Buffer, SizeRecived);
				break;

			case Disconnected :
				ProtocolSetStatus(NETWORK_DISCONNECTED);
				break;

			default :
				ProtocolSetStatus(NETWORK_ERROR);
				break;
		}
	}
}

////////////////////////////////////////////////////////////
/// Protocol parser
////////////////////////////////////////////////////////////
void ProtocolParser(Int8 * Data, Int32 DataSize)
{
	Int32 i; UInt8 ByteData;

	for (i = 0; i < DataSize; i++)
	{
		ByteData = Data[i];

		switch (NetworkConnection.Parser)
		{
			case PARSER_WAITING_PREAMBLE :
				if (ByteData ==  NETWORK_PREAMBLE)
					NetworkConnection.Parser = PARSER_WAITING_TYPE;
				break;

			case PARSER_WAITING_TYPE :
				NetworkConnection.Type = ByteData;
				NetworkConnection.Parser = PARSER_WAITING_CODE;
				break;

			case PARSER_WAITING_CODE :
				NetworkConnection.Code = ByteData;
				NetworkConnection.Parser = PARSER_WAITING_LENGTH;
				break;

			case PARSER_WAITING_LENGTH :
				NetworkConnection.PacketLength = ByteData;
				NetworkConnection.Parser = PARSER_WAITING_CLENGTH;
				break;

			case PARSER_WAITING_CLENGTH :

				if (NetworkConnection.PacketLength + ByteData != 255)
				{
					NetworkConnection.Parser = PARSER_WAITING_PREAMBLE;
	                break;
				}

				if (ByteData == 0)
				{
					NetworkConnection.Parser = PARSER_WAITING_PREAMBLE;
					ProtocolHandleData();
					break;
				}

				if (ByteData > 0 )
				{
					NetworkConnection.CurrentLength = 0;
					NetworkConnection.Parser = PARSER_WAITING_DATA;
				}
				break;

			case PARSER_WAITING_DATA :
				NetworkConnection.Packet.Buffer[NetworkConnection.CurrentLength++] = ByteData;

				if (NetworkConnection.CurrentLength == NetworkConnection.PacketLength)
				{
					NetworkConnection.Parser = PARSER_WAITING_PREAMBLE;

					if (ProtocolHandleData() == false)
					{
						// Close the connection if the data is corrupted
						if (!ProtocolDisconnect())
						{
							MessageError("ProtocolParser", "Error when the socket was closing.");
						}
						return;
					}
				}
				break;
		}
	}
}

////////////////////////////////////////////////////////////
/// Handles incoming data socket
////////////////////////////////////////////////////////////
bool ProtocolHandleData()
{
	if (UserGetData()->Logged == false)
	{
		switch (NetworkConnection.Type)
		{
			case NETWORK_TYPE_LOGIN :
				switch (NetworkConnection.Code)
				{
					case NETWORK_CODE_RESULT :
						return HandlePacketResult();

					case NETWORK_CODE_ACCOUNT :
						return HandlePacketAccount();
				}

			default : // If packet type is another else...
				return false;
		}
	}
	else
	{
		switch (NetworkConnection.Type)
		{
			// More packets ...
		}
	}

    return false;
}

////////////////////////////////////////////////////////////
/// Handle the result packet
////////////////////////////////////////////////////////////
bool HandlePacketResult()
{
	// Check the integrity
	if (NetworkConnection.PacketLength != sizeof(struct PacketBool))
		return false;

	// Check if the request code was sucessful
	if (NetworkConnection.Packet.Boolean.Value == true)
	{
		struct PacketLogin LoginStruct;

		strcpy(LoginStruct.User, UserGetData()->Name);
		strcpy(LoginStruct.Password, UserGetData()->Password);

		ProtocolSendData(NETWORK_TYPE_LOGIN, NETWORK_CODE_LOGIN, &LoginStruct, sizeof(struct PacketLogin));
		return true;
	}
	else
	{
		MessageError("Error", "Server connection Error!\n");
		return false;
	}
}

////////////////////////////////////////////////////////////
/// Handle the account packet
////////////////////////////////////////////////////////////
bool HandlePacketAccount()
{
	UInt32 i;

	// Update account state
	GameSetStatus(GAME_ACCOUNT);

	// Get the data
	for (i = 0; i < 6; i++)
	{
		sreadfield(UserGetData()->Players[i].Name, NetworkConnection.Packet.Account.Buffer, ';', i);
		
		if (!strlen(UserGetData()->Players[i].Name))
			break;
	
		UserGetData()->PlayersSize++;
	}

	// Show accounts
	AccountOpen();

	return true;
}

////////////////////////////////////////////////////////////
/// Protocol send data procediment
////////////////////////////////////////////////////////////
void ProtocolSendData(UInt8 Type, UInt8 Code, void * Data, Int8 Size)
{
	if (ProtocolGetStatus() == NETWORK_CONNECTED)
	{
		// Buffer data
		Int8 Buffer[4096];

		// Socket data
		struct SocketData Socket;

		// Set buffer initial data
		Buffer[0] = NETWORK_PREAMBLE;
		Buffer[1] = Type;
		Buffer[2] = Code;
		Buffer[3] = Size;
		Buffer[4] = ~Size;

		// Get the client socket
		ProtocolGetSocket(&Socket);

		// Put data into buffer
		if ( Size > 0 )
		{
			memcpy(&Buffer[5], Data, Size);
		}

		// Send data to server
		SocketSend(&Socket, &Buffer[0], Size + 5);

		/// \example SendData(NETWORK_TYPE_LOGIN, NETWORK_CODE_REQUEST, &RequestStruct, sizeof(struct PacketRequest));
	}
}

////////////////////////////////////////////////////////////
/// Packeting protocol
////////////////////////////////////////////////////////////

#include <System/MD5.h> // Packet headers

void PacketSendRequest()
{
	struct PacketRequest RequestStruct;

	// Get MD5 File
	#ifndef DEBUG
        #ifdef SYSTEM_WINDOWS
            MD5File("Client.exe", RequestStruct.MD5);
        #else
            MD5File("Client", RequestStruct.MD5);
        #endif
	#else
		strcpy((char*)RequestStruct.MD5, "abcdefghijklmnop");
	#endif

	// Update application version
	RequestStruct.AppMajor		= APP_MAJOR_VERSION;
	RequestStruct.AppMinor		= APP_MINOR_VERSION;
	RequestStruct.AppRevision	= APP_REVISION_VERSION;

	// Send packet
	ProtocolSendData(NETWORK_TYPE_LOGIN, NETWORK_CODE_REQUEST, &RequestStruct, sizeof(struct PacketRequest));
}
