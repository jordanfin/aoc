#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Network/SocketHelper.h>

////////////////////////////////////////////////////////////
/// Socket wraps a socket using TCP protocol to
/// send data safely (but a bit slower)
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
struct SocketData {
	SocketType 	Type;		///< Socket descriptor
	char *		Adress;		///< Socket IP Adress
	bool 		IsBlocking;	///< Is the socket blocking or non-blocking ?
};

////////////////////////////////////////////////////////////
/// Connect to another computer on a specified port
///
/// \param Port :        Port to use for transfers (warning : ports < 1024 are reserved)
/// \param HostAddress : IP Address of the host to connect to
/// \param Timeout :     Maximum time to wait, in seconds (0 by default : no timeout) (this parameter is ignored for non-blocking sockets)
///
/// \return True if operation has been successful
///
////////////////////////////////////////////////////////////
enum SocketStatus SocketConnect(struct SocketData * Socket, unsigned short Port, char* HostAddress, float Timeout);

////////////////////////////////////////////////////////////
/// Listen to a specified port for incoming data or connections
///
/// \param Port : Port to listen to
///
/// \return True if operation has been successful
///
////////////////////////////////////////////////////////////
bool SocketListenPort(struct SocketData * Socket, unsigned short Port);

////////////////////////////////////////////////////////////
/// Wait for a connection (must be listening to a port).
/// This function will block if the socket is blocking
///
/// \param Connected : Socket containing the connection with the connected client
/// \param Address :   Pointer to an address to fill with client infos (NULL by default)
///
/// \return Status code
///
////////////////////////////////////////////////////////////
enum SocketStatus SocketAccept(struct SocketData * SocketListener, struct SocketData * Connected);

////////////////////////////////////////////////////////////
/// Send an array of bytes to the host (must be connected first)
///
/// \param Data : Pointer to the bytes to send
/// \param Size : Number of bytes to send
///
/// \return Status code
///
////////////////////////////////////////////////////////////
enum SocketStatus SocketSend(struct SocketData * Socket, Int8 * Data, Int32 Size);

////////////////////////////////////////////////////////////
/// Receive an array of bytes from the host (must be connected first).
/// This function will block if the socket is blocking
///
/// \param Data :         Pointer to a byte array to fill (make sure it is big enough)
/// \param MaxSize :      Maximum number of bytes to read
/// \param SizeReceived : Number of bytes received
///
/// \return Status code
///
////////////////////////////////////////////////////////////
enum SocketStatus SocketReceive(struct SocketData * Socket, Int8 * Data, Int32 MaxSize, Int32* SizeReceived);

////////////////////////////////////////////////////////////
/// Close the socket
///
/// \return True if operation has been successful
///
////////////////////////////////////////////////////////////
bool SocketClose(struct SocketData * Socket);

////////////////////////////////////////////////////////////
/// Check if the socket is in a valid state ; this function
/// can be called any time to check if the socket is OK
///
/// \return True if the socket is valid
///
////////////////////////////////////////////////////////////
bool SocketIsValid(struct SocketData * Socket);

#endif // NETWORK_SOCKET_H
