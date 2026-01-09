//
// Created by Ian on 5-1-2026.
//

#ifndef PERFNETPROXY_INTERNALSOCKET_H
#define PERFNETPROXY_INTERNALSOCKET_H

#include <winsock2.h>
#include <mswsock.h>

struct InternalListeningSocket {
    SOCKET socket {INVALID_SOCKET};
    LPFN_ACCEPTEX acceptExFunc {};
};

struct InternalClientSocket {
    SOCKET socket {INVALID_SOCKET};
};

#endif //PERFNETPROXY_INTERNALSOCKET_H