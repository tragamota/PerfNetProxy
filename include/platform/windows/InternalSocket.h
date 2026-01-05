//
// Created by Ian on 5-1-2026.
//

#ifndef PERFNETPROXY_INTERNALSOCKET_H
#define PERFNETPROXY_INTERNALSOCKET_H

#include <winsock2.h>

struct InternalSocket {
    SOCKET socket;
    sockaddr_storage address;
};

#endif //PERFNETPROXY_INTERNALSOCKET_H