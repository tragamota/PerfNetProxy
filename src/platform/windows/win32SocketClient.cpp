//
// Created by Ian on 9-1-2026.
//

#include <iostream>
#include <ostream>
#include <span>

#include "socketClient.hpp"

#include <winsock2.h>

#include "ioContext.hpp"

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
    if (m_InternalSocket.socket == INVALID_SOCKET)
        return;

    if (auto closeRet = ::closesocket(m_InternalSocket.socket); closeRet == SOCKET_ERROR) {
        std::cout << "Close socket failed " << WSAGetLastError() << std::endl;
    }

    m_InternalSocket.socket = INVALID_SOCKET;
}

void SocketClient::queueSendTask(const std::span<const uint8_t> message) const {
    auto* sendContext = new SendContext{};

    sendContext->operation = IOOperation::Send;
    sendContext->buffer = new uint8_t[message.size()];

    std::ranges::copy(message, sendContext->buffer);

    sendContext->sendBuffer.buf = reinterpret_cast<CHAR*>(sendContext->buffer);
    sendContext->sendBuffer.len = message.size();

    const auto ret = WSASend(m_InternalSocket.socket,
                       &sendContext->sendBuffer,
                       1,
                       nullptr,
                       0,
                       &sendContext->overlapped,
                       nullptr);

    if (ret == SOCKET_ERROR) {
        std::cout << "WSASend failed " << WSAGetLastError() << std::endl;
    }
}

void SocketClient::queueReceiveTask(const size_t receiveBufferSize) const {
    auto* receiveContext = new ReceiveContext {};

    receiveContext->operation = IOOperation::Receive;
    receiveContext->buffer = new uint8_t[receiveBufferSize];

    receiveContext->sendBuffer.buf = reinterpret_cast<CHAR*>(receiveContext->buffer);
    receiveContext->sendBuffer.len = receiveBufferSize;

    int flags = 0;

    const auto ret = WSARecv(m_InternalSocket.socket,
                           &receiveContext->sendBuffer,
                           1,
                           nullptr,
                           reinterpret_cast<LPDWORD>(&flags),
                           &receiveContext->overlapped,
                           nullptr);

    if (ret == SOCKET_ERROR) {
        std::cout << "WSARecv failed " << WSAGetLastError() << std::endl;
    }
}

void SocketClient::disconnect() const {
    if (m_InternalSocket.socket == INVALID_SOCKET)
        return;

    if (auto ret = ::shutdown(m_InternalSocket.socket, SD_BOTH); ret == SOCKET_ERROR) {
        std::cout << "shutdown failed " << WSAGetLastError() << std::endl;
    }
}
