#ifndef NETWORK_PROTOCOL_H
#define NETWORK_PROTOCOL_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
// Protocol parser definitions
////////////////////////////////////////////////////////////
#define PARSER_WAITING_PREAMBLE		0
#define PARSER_WAITING_TYPE			1
#define PARSER_WAITING_CODE			2
#define PARSER_WAITING_LENGTH		3
#define PARSER_WAITING_CLENGTH		4
#define PARSER_WAITING_DATA			5

////////////////////////////////////////////////////////////
// Protocol data definitions
////////////////////////////////////////////////////////////
#define NETWORK_BUFFER_LENGTH		253			///< (Data Size - Packet Header)
#define NETWORK_PREAMBLE			0x73		///< Parser Preamble
#define NETWORK_CONNECTION_PORT		12993		///< Server Port
#define NETWORK_CONNECTION_IP		"127.0.0.1"	///< Server IP

////////////////////////////////////////////////////////////
// Protocol connection status definitions
////////////////////////////////////////////////////////////
#define NETWORK_CONNECTED			0x00
#define NETWORK_DISCONNECTED		0x01
#define NETWORK_ERROR				0x02
#define NETWORK_DESTROYED			0x03

////////////////////////////////////////////////////////////
// Protocol packet definitions
////////////////////////////////////////////////////////////
#define NETWORK_TYPE_LOGIN			0x01
	#define NETWORK_CODE_REQUEST		0x01
	#define NETWORK_CODE_RESULT			0x02
	#define NETWORK_CODE_LOGIN			0x03
	#define NETWORK_CODE_ACCOUNT		0x04
	#define NETWORK_CODE_ACC_ERROR		0x05
	#define NETWORK_CODE_PLAYER			0x06

#define NETWORK_TYPE_ERROR			0x02
	#define NETWORK_CODE_ERROR			0x01

#define NETWORK_TYPE_WALK			0x03

#define NETWORK_TYPE_TALK			0x04

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct PacketRequest
{
    unsigned char MD5[16];
    char AppMajor;
    char AppMinor;
    char AppRevision;
};

struct PacketLogin
{
    char User[32];
    char Password[32];
};

struct PacketBool
{
	UInt8 Value; ///< true 1 / false 0
};

struct PacketError
{
	Int16 Code;
    char  Text[128];
};

struct PacketAccount
{
	char Buffer[0xC6];
};

struct PacketPlayer
{
	Int8 Index;
};

union IncomingPacket
{
    struct PacketLogin		Login;
    struct PacketBool		Boolean;
    struct PacketAccount	Account;
    struct PacketError		Error;
    char 					Buffer[NETWORK_BUFFER_LENGTH];
};

struct Connection
{
    Int32	Parser;					///< Parser state
    UInt8	Type;					///< Packet type
	UInt8	Code;					///< Packet type code
    UInt8 	PacketLength;			///< Packet data length
	UInt8	CurrentLength;			///< Current length data
	union IncomingPacket Packet;	///< Incoming Packet data
};

struct Connection NetworkConnection; ///< Connection declare

////////////////////////////////////////////////////////////
/// Switch on/off the socket listener
///
/// \return Nothing
////////////////////////////////////////////////////////////
// void ConnectionSwitch(bool value);

////////////////////////////////////////////////////////////
/// Initialize the connection with server
///
/// \return true if the connection has been successful
////////////////////////////////////////////////////////////
void ProtocolInitialize();

////////////////////////////////////////////////////////////
/// Destroy the connection with server
///
/// \return true if the connection has been destroyed
////////////////////////////////////////////////////////////
void ProtocolDestroy();

////////////////////////////////////////////////////////////
/// Connect to the server
////////////////////////////////////////////////////////////
void ProtocolConnect();

////////////////////////////////////////////////////////////
/// Disconnect from the server
////////////////////////////////////////////////////////////
bool ProtocolDisconnect();

////////////////////////////////////////////////////////////
/// Get the client socket
////////////////////////////////////////////////////////////
void ProtocolGetSocket(struct SocketData * Copy);

////////////////////////////////////////////////////////////
/// Get the client connection status
////////////////////////////////////////////////////////////
UInt32 ProtocolGetStatus();

////////////////////////////////////////////////////////////
/// Set the client connection status
////////////////////////////////////////////////////////////
void ProtocolSetStatus(UInt32 Status);

////////////////////////////////////////////////////////////
/// Protocol send data procediment
///
/// \param Type : Protocol type ID
/// \param Code : Protocol type code ID
/// \param Data : Data to send
/// \param Size : Size of data
///
/// \return Nothing
////////////////////////////////////////////////////////////
void ProtocolSendData(UInt8 Type, UInt8 Code, void * Data, Int8 Size);

////////////////////////////////////////////////////////////
/// Listen procediment
///
/// \return Nothing
////////////////////////////////////////////////////////////
void ProtocolListen();

////////////////////////////////////////////////////////////
/// Parser procediment
///
/// \param Data : buffer recive data
/// \param DataSize : buffer size data
///
/// \return Nothing
////////////////////////////////////////////////////////////
void ProtocolParser(Int8 * Data, Int32 DataSize);

////////////////////////////////////////////////////////////
/// Incoming data manager procediment
///
/// \return true : If the managment was sucessful
////////////////////////////////////////////////////////////
bool ProtocolHandleData();

////////////////////////////////////////////////////////////
/// Packeting protocol
////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
/// Protocol documentation
///
/// Data structure
///
/// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
/// | Preamble[0] = 0x73 |  Type[1] | Code[2] | Size[3] | ~Size[4] | Data[<255] |
/// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///
/// Direction	Type					Code					Data
/// C -> S		NETWORK_TYPE_LOGIN		NETWORK_CODE_REQUEST	struct PacketRequest
/// S -> C		NETWORK_TYPE_LOGIN		NETWORK_CODE_RESULT		struct PacketBool
/// C -> S		NETWORK_TYPE_LOGIN		NETWORK_CODE_LOGIN		struct PacketLogin
///
/// S -> C		NETWORK_TYPE_ERROR		NETWORK_CODE_ERROR		struct PacketError
///
/////////////////////////////////////////////////////////////////////////////////

void PacketSendRequest(); ///< Request from server

#endif // NETWORK_PROTOCOL_H
