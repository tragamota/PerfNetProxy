//
// Created by Ian on 5-1-2026.
//

#include <socketListener.hpp>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../../../include/platform/windows/ioContext.hpp"

void bindAcceptFunction(InternalListeningSocket &internalSocket) {
    uint32_t bytesReturned = 0;
    GUID AcceptExID = WSAID_ACCEPTEX;
    constexpr auto AcceptExIDSize = sizeof(AcceptExID);
    constexpr auto AcceptExFuncSize = sizeof(internalSocket.acceptExFunc);

    const auto returnValue = WSAIoctl(
        internalSocket.socket,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &AcceptExID, AcceptExIDSize,
        &internalSocket.acceptExFunc, AcceptExFuncSize,
        reinterpret_cast<LPDWORD>(&bytesReturned),
        nullptr,
        nullptr);

    if (returnValue == SOCKET_ERROR) {
        throw std::runtime_error(std::format("Failed to get acceptEx function pointer {}", WSAGetLastError()));
    }
}

void SocketListener::cleanup() {
    if (m_InternalSocket.socket != INVALID_SOCKET) {
        closesocket(m_InternalSocket.socket);
        m_InternalSocket.socket = INVALID_SOCKET;
    }
}

SocketListener::SocketListener(const InternetProtocolVersion &protocolVersion) : m_ProtocolVersion(protocolVersion) {
    constexpr auto internetProtocol = IPPROTO_TCP;

    const auto addressFamily = protocolVersion == InternetProtocolVersion::IPv4 ? AF_INET : AF_INET6;

    m_InternalSocket.socket = WSASocketW(addressFamily,
                                         SOCK_STREAM,
                                         internetProtocol,
                                         nullptr,
                                         0,
                                         WSA_FLAG_OVERLAPPED);

    if (m_InternalSocket.socket == INVALID_SOCKET) {
        cleanup();
        throw std::runtime_error(std::format("Failed to create socket with following error {}", WSAGetLastError()));
    }

    if (protocolVersion == InternetProtocolVersion::IPv6) {
        setsockopt(m_InternalSocket.socket, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<char *>(1), sizeof(int));
    }
}

void SocketListener::bind(const std::string &bind_address, const uint16_t port) {
    addrinfo hints{};
    addrinfo *conversionResult = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(bind_address.data(), std::to_string(port).data(), &hints, &conversionResult) != 0) {
        throw std::runtime_error(std::format("Failed to convert address {}:{}", bind_address, port));
    }

    if (::bind(m_InternalSocket.socket, conversionResult->ai_addr, static_cast<int>(conversionResult->ai_addrlen)) ==
        SOCKET_ERROR) {
        freeaddrinfo(conversionResult);
        throw std::runtime_error(std::format("Failed to bind socket {}", WSAGetLastError()));
    }

    bindAcceptFunction(m_InternalSocket);

    freeaddrinfo(conversionResult);
}

void SocketListener::listen() const {
    if (::listen(m_InternalSocket.socket, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error(std::format("Failed to listen on socket {}", WSAGetLastError()));
    }
}

void SocketListener::acceptClient() const {
    const auto addressFamily = m_ProtocolVersion == InternetProtocolVersion::IPv4 ? AF_INET : AF_INET6;

    auto clientSocket = WSASocketW(
        addressFamily,
        SOCK_STREAM,
        IPPROTO_TCP,
        nullptr,
        0,
        WSA_FLAG_OVERLAPPED
    );

    auto *acceptContext = new AcceptContext{};

    acceptContext->operation = IOOperation::Accept;

    if (m_ProtocolVersion == InternetProtocolVersion::IPv6) {
        constexpr auto acceptAddressBufferSize = sizeof(sockaddr_in6) + 16;

        acceptContext->outputBufferSize = acceptAddressBufferSize;
    } else {
        constexpr auto acceptAddressBufferSize = sizeof(sockaddr_in) + 16;

        acceptContext->outputBufferSize = acceptAddressBufferSize;
    }

    acceptContext->outputBuffer = new uint8_t[acceptContext->outputBufferSize * 2]{};

    m_InternalSocket.acceptExFunc(m_InternalSocket.socket,
        clientSocket,
        acceptContext->outputBuffer,
        0,
        acceptContext->outputBufferSize,
        acceptContext->outputBufferSize,
        nullptr,
        &acceptContext->overlapped);
}

void SocketListener::close() {
    ::shutdown(m_InternalSocket.socket, SD_BOTH);
    cleanup();
}
