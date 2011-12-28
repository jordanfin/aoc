#ifndef NETWORK_PROTOCOL_H
#define NETWORK_PROTOCOL_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <Network/Socket.h>
#include <System/Error.h>
#include <System/Mutex.h>
#include <System/Thread.h>
#include <System/Platform.h>
#include <Database/Default.h>

////////////////////////////////////////////////////////////
// Protocol parser definitions
////////////////////////////////////////////////////////////
#define PARSER_WAITING_PREAMBLE			0x00
#define PARSER_WAITING_TYPE				0x01
#define PARSER_WAITING_CODE				0x02
#define PARSER_WAITING_LENGTH			0x03
#define PARSER_WAITING_CLENGTH			0x04
#define PARSER_WAITING_DATA				0x05

////////////////////////////////////////////////////////////
// Protocol data definitions
////////////////////////////////////////////////////////////
#define NETWORK_MAX_LENGTH				0xFD
#define NETWORK_PREAMBLE				0x73

////////////////////////////////////////////////////////////
// Protocol error
////////////////////////////////////////////////////////////
#define NETWORK_ERROR_USR_LOG			0x00
#define NETWORK_ERROR_ACC_PASS			0x01
#define NETWORK_ERROR_ACC_BAN			0x02

////////////////////////////////////////////////////////////
// Protocol packet definitions
////////////////////////////////////////////////////////////
#define NETWORK_TYPE_LOGIN				0x01
	#define NETWORK_CODE_REQUEST		0x01
	#define NETWORK_CODE_RESULT			0x02
	#define NETWORK_CODE_LOGIN			0x03
	#define NETWORK_CODE_ACCOUNT		0x04
	#define NETWORK_CODE_ACC_ERROR		0x05
	#define NETWORK_CODE_PLAYER			0x06

#define NETWORK_TYPE_ERROR				0x02
	#define NETWORK_CODE_ERROR			0x01

#define NETWORK_TYPE_WALK				0x03

#define NETWORK_TYPE_TALK				0x04

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
	UInt32 Code;
	// char Text[0xFF];
};

struct PacketPlayer
{
	Int8 Index;
};

union PacketData
{
	struct PacketRequest	Request;
    struct PacketLogin		Login;
    struct PacketBool		Boolean;
    struct PacketError		Error;
	struct PacketPlayer		Player;
    char 					Buffer[NETWORK_MAX_LENGTH];
};

struct ConnectionData
{
    Int32	Parser;					///< Parser state
    UInt8	Type;					///< Packet type
	UInt8	Code;					///< Packet type code
    UInt8 	PacketLength;			///< Packet data length
	UInt8	CurrentLength;			///< Current lenght data
	union	PacketData Packet;		///< Packet data
	struct	SocketData Socket;		///< Socket data
};

////////////////////////////////////////////////////////////
/// Protocol initializer
////////////////////////////////////////////////////////////
bool ProtocolInitialize(UInt16 Port);

////////////////////////////////////////////////////////////
/// Protocol destroyer
////////////////////////////////////////////////////////////
bool ProtocolDestroy();

////////////////////////////////////////////////////////////
/// Socket listener proc
////////////////////////////////////////////////////////////
void ProtocolListener();

////////////////////////////////////////////////////////////
/// Parser procediment
////////////////////////////////////////////////////////////
bool ProtocolParser(struct UserData * User, UInt8 * Data, Int32 DataSize);

////////////////////////////////////////////////////////////
/// Incoming user data manager procediment
////////////////////////////////////////////////////////////
bool ProtocolHandleUser(struct UserData * User);

////////////////////////////////////////////////////////////
/// Protocol send error
////////////////////////////////////////////////////////////
void ProtocolSendError(struct UserData * User, UInt32 Code);

////////////////////////////////////////////////////////////
/// Protocol send data procediment
///
/// param User : User pointer
/// param Type : Protocol type ID
/// param Code : Protocol type code ID
/// param Data : Data to send
/// param Size : Size of data
////////////////////////////////////////////////////////////
void ProtocolSendData(struct UserData * User, UInt8 Type, UInt8 Code, void * Data, Int8 Size);

#endif // NETWORK_PROTOCOL_H
