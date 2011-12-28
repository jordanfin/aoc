#ifndef NETWORK_SOCKETHELPER_H
#define NETWORK_SOCKETHELPER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>


#if defined(SYSTEM_WINDOWS)

	#include <winsock2.h>

	////////////////////////////////////////////////////////////
	// Define some socket types
	////////////////////////////////////////////////////////////
	typedef SOCKET	SocketType;
	typedef int		SocketLengthType;

#elif defined(SYSTEM_LINUX) || defined(SYSTEM_MACOS) || defined(SYSTEM_FREEBSD)

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>

    ////////////////////////////////////////////////////////////
    // Define some socket types
    ////////////////////////////////////////////////////////////
    typedef int			SocketType;
    typedef socklen_t	SocketLengthType;

#endif


////////////////////////////////////////////////////////////
/// Enumeration of status returned by socket functions
////////////////////////////////////////////////////////////
enum SocketStatus
{
	Done,         ///< The socket has sent / received the data
	NotReady,     ///< The socket is not ready to send / receive data yet
	Disconnected, ///< The TCP socket has been disconnected
	Error         ///< An unexpected error happened
};

////////////////////////////////////////////////////////////
/// This class defines helper functions to do all the
/// non-portable socket stuff. This class is meant for internal
/// use only
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Windows needs some initialization and cleanup to get
/// sockets working properly..
////////////////////////////////////////////////////////////
void SocketHelperInitialize(); void SocketHelperCleanup();

////////////////////////////////////////////////////////////
/// Return the value of the invalid socket
///
/// \return Unique value of the invalid socket
///
////////////////////////////////////////////////////////////
SocketType InvalidSocket();

////////////////////////////////////////////////////////////
/// Close / destroy a socket
///
/// \param Socket : Socket to close
///
/// \return True on success
///
////////////////////////////////////////////////////////////
bool SocketHelperClose(SocketType Socket);

////////////////////////////////////////////////////////////
/// Set a socket as blocking or non-blocking
///
/// \param Socket : Socket to modify
/// \param Block :  New blocking state of the socket
///
////////////////////////////////////////////////////////////
void SocketHelperSetBlocking(SocketType Socket, bool Block);

////////////////////////////////////////////////////////////
/// Get the last socket error status
///
/// \return Status corresponding to the last socket error
///
////////////////////////////////////////////////////////////
enum SocketStatus SocketHelperGetErrorStatus();

#endif // NETWORK_SOCKETHELPER_H
