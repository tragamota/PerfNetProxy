//
// Created by Ian on 9-1-2026.
//

#include <iostream>
#include <ostream>
#include <span>

#include <winsock2.h>

#include "socketClient.hpp"
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
    auto sendContext = std::make_unique<SendContext>();

    sendContext->buffer = std::make_unique<uint8_t[]>(message.size());
    sendContext->sendBuffer.buf = reinterpret_cast<CHAR*>(sendContext->buffer.get());
    sendContext->sendBuffer.len = message.size();

    std::ranges::copy(message, sendContext->buffer.get());

    const auto ret = WSASend(m_InternalSocket.socket,
                       &sendContext->sendBuffer,
                       1,
                       nullptr,
                       0,
                       &sendContext->baseContext.overlapped,
                       nullptr);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cout << "WSASend failed " << WSAGetLastError() << std::endl;

        return;
    }

    sendContext.release();
}

void SocketClient::queueReceiveTask(const size_t receiveBufferSize) const {
    auto receiveContext = std::make_unique<ReceiveContext>();

    receiveContext->buffer = std::make_unique<uint8_t[]>(receiveBufferSize);

    receiveContext->sendBuffer.buf = reinterpret_cast<CHAR*>(receiveContext->buffer.get());
    receiveContext->sendBuffer.len = receiveBufferSize;

    int flags = 0;

    const auto ret = WSARecv(m_InternalSocket.socket,
                           &receiveContext->sendBuffer,
                           1,
                           nullptr,
                           reinterpret_cast<LPDWORD>(&flags),
                           &receiveContext->baseContext.overlapped,
                           nullptr);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cout << "WSARecv failed " << WSAGetLastError() << std::endl;

        return;
    }

    receiveContext.release();
}

void SocketClient::disconnect() const {
    if (m_InternalSocket.socket == INVALID_SOCKET)
        return;

    if (auto ret = ::shutdown(m_InternalSocket.socket, SD_BOTH); ret == SOCKET_ERROR) {
        std::cout << "shutdown failed " << WSAGetLastError() << std::endl;
    }
}
