#ifndef ANKER_SOCKET_HPP
#define ANKER_SOCKET_HPP

// Minimal portability layer for the BSD-socket code in AnkerMqtt / AnkerPppp.
// On POSIX these are thin wrappers around the usual calls (behaviour unchanged);
// on Windows they map to Winsock. Sockets are kept as `int` like on POSIX: Winsock
// handles fit in 32 bits and INVALID_SOCKET converts to -1, so `fd < 0` still works.

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa / inet_addr
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <BaseTsd.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif

namespace AnkerNet {

#ifdef _WIN32
using ssize_t = SSIZE_T;
#endif

namespace sock {

// Winsock has to be initialised once per process before the first socket call.
// No-op on POSIX.
inline void init()
{
#ifdef _WIN32
    static const bool ok = [] {
        WSADATA data;
        return WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }();
    (void)ok;
#endif
}

inline void closeSocket(int fd)
{
#ifdef _WIN32
    ::closesocket(static_cast<SOCKET>(fd));
#else
    ::close(fd);
#endif
}

// SO_RCVTIMEO takes a struct timeval on POSIX but a DWORD of milliseconds on Windows.
inline void setRecvTimeout(int fd, int ms)
{
#ifdef _WIN32
    DWORD t = static_cast<DWORD>(ms);
    setsockopt(static_cast<SOCKET>(fd), SOL_SOCKET, SO_RCVTIMEO,
        reinterpret_cast<const char*>(&t), sizeof(t));
#else
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif
}

// Creates an IPv4 UDP socket. On Windows an ICMP "port unreachable" (common while
// broadcasting / hole punching) would otherwise make the next recvfrom() fail with
// WSAECONNRESET, so that reporting is switched off.
inline int udpSocket()
{
    init();
    int fd = static_cast<int>(::socket(AF_INET, SOCK_DGRAM, 0));
#ifdef _WIN32
    if (fd >= 0) {
        BOOL report = FALSE;
        DWORD bytes = 0;
        // SIO_UDP_CONNRESET = _WSAIOW(IOC_VENDOR, 12), from <mstcpip.h>.
        WSAIoctl(static_cast<SOCKET>(fd), _WSAIOW(IOC_VENDOR, 12), &report, sizeof(report),
            nullptr, 0, &bytes, nullptr, nullptr);
    }
#endif
    return fd;
}

// Waits up to `ms` for `fd` to become readable. Returns false only on timeout;
// errors return true so the following read reports them.
inline bool waitReadable(int fd, int ms)
{
    fd_set rs;
    FD_ZERO(&rs);
#ifdef _WIN32
    FD_SET(static_cast<SOCKET>(fd), &rs);
#else
    FD_SET(fd, &rs);
#endif
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    return ::select(fd + 1, &rs, nullptr, nullptr, &tv) != 0;
}

} // namespace sock
} // namespace AnkerNet

#endif // ANKER_SOCKET_HPP
