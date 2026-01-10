//
// Created by Ian on 9-1-2026.
//

#include "socketClient.hpp"

#include <winsock2.h>

SocketClient::SocketClient(const InternetProtocolVersion protocolVersion) {
    const auto addressFamily = protocolVersion == InternetProtocolVersion::IPv4 ? AF_INET : AF_INET6;

    m_InternalSocket.socket = WSASocketW(
        addressFamily,
        SOCK_STREAM,
        IPPROTO_TCP,
        nullptr,
        0,
        WSA_FLAG_OVERLAPPED
    );
}

SocketClient::~SocketClient() {
    if (m_InternalSocket.socket != INVALID_SOCKET) {
        closesocket(m_InternalSocket.socket);
        m_InternalSocket.socket = INVALID_SOCKET;
    }
}

void SocketClient::queueSendTask(uint8_t &message) const {

}

void SocketClient::disconnect() const {
    ::shutdown(m_InternalSocket.socket, SD_BOTH);
}
