////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Network/SocketHelper.h>
#include <errno.h>
#include <fcntl.h>


////////////////////////////////////////////////////////////
/// In Unix this procediment not performs
////////////////////////////////////////////////////////////
void SocketHelperInitialize()
{
	// Nothing..
}

////////////////////////////////////////////////////////////
/// Clean...
////////////////////////////////////////////////////////////
void SocketHelperCleanup()
{
	// Nothing..
}

////////////////////////////////////////////////////////////
/// Return the value of the invalid socket
////////////////////////////////////////////////////////////
SocketType InvalidSocket()
{
    return -1;
}


////////////////////////////////////////////////////////////
/// Close / destroy a socket
////////////////////////////////////////////////////////////
bool SocketHelperClose(SocketType Socket)
{
    return (close(Socket) != -1);
}


////////////////////////////////////////////////////////////
/// Set a socket as blocking or non-blocking
////////////////////////////////////////////////////////////
void SocketHelperSetBlocking(SocketType Socket, bool Block)
{
    Int32 Status = fcntl(Socket, F_GETFL);

    if (Block)
        fcntl(Socket, F_SETFL, Status & ~O_NONBLOCK);
    else
        fcntl(Socket, F_SETFL, Status | O_NONBLOCK);
}


////////////////////////////////////////////////////////////
/// Get the last socket error status
////////////////////////////////////////////////////////////
enum SocketStatus SocketHelperGetErrorStatus()
{
    // The followings are sometimes equal to EWOULDBLOCK,
    // so we have to make a special case for them in order
    // to avoid having double values in the switch case
    if ((errno == EAGAIN) || (errno == EINPROGRESS))
        return NotReady;

    switch (errno)
    {
        case EWOULDBLOCK :  return NotReady;
        case ECONNABORTED : return Disconnected;
        case ECONNRESET :   return Disconnected;
        case ETIMEDOUT :    return Disconnected;
        case ENETRESET :    return Disconnected;
        case ENOTCONN :     return Disconnected;
        default :           return Error;
    }
}
