#include "networking/windows/winsock.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "core/error_handling.h"

namespace iris
{
Winsock::Winsock()
{
    WSADATA data = {0};

    ensure(::WSAStartup(MAKEWORD(2, 2), &data) == 0, "failed to init winsock");
}

/**
 * Cleanup winsock.
 */
Winsock::~Winsock()
{
    ::WSACleanup();
}

}
