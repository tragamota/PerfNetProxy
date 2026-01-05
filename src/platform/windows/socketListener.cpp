//
// Created by Ian on 5-1-2026.
//

#include <socketListener.hpp>
#include <format>
#include <stdexcept>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

void SocketListener::cleanup() const {
    if (m_InternalSocket->socket != INVALID_SOCKET) {
        closesocket(m_InternalSocket->socket);
        m_InternalSocket->socket = INVALID_SOCKET;
    }
}

SocketListener::SocketListener(const InternetProtocolVersion &protocolVersion) : m_InternalSocket(
    std::make_unique<InternalSocket>()) {
    const auto addressFamily = protocolVersion == InternetProtocolVersion::IPv4 ? AF_INET : AF_INET6;
    const auto internetProtocol = IPPROTO_TCP;
    m_InternalSocket->socket = WSASocketW(addressFamily, SOCK_STREAM, internetProtocol, nullptr, 0,
                                          WSA_FLAG_OVERLAPPED);
    if (m_InternalSocket->socket == INVALID_SOCKET) {
        cleanup();
        throw std::runtime_error(std::format("Failed to create socket with following error {}", WSAGetLastError()));
    }
    if (protocolVersion == InternetProtocolVersion::IPv6) {
        setsockopt(m_InternalSocket->socket, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<char *>(1), sizeof(int));
    }
}

SocketListener::~SocketListener() { cleanup(); }

SocketListener::SocketListener(SocketListener &&other) noexcept : m_InternalSocket(std::move(other.m_InternalSocket)) {
}

SocketListener &SocketListener::operator=(SocketListener &&other) noexcept {
    if (this != &other) { m_InternalSocket = std::move(other.m_InternalSocket); }
    return *this;
}

void SocketListener::bind(const std::string_view &bind_address, const uint16_t port) const {
    addrinfo hints{};
    addrinfo *conversionResult = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(bind_address.data(), std::to_string(port).data(), &hints, &conversionResult) != 0) {
        throw std::runtime_error(std::format("Failed to convert address {}:{}", bind_address, port));
    }
    if (::bind(m_InternalSocket->socket, conversionResult->ai_addr, static_cast<int>(conversionResult->ai_addrlen)) ==
        SOCKET_ERROR) {
        freeaddrinfo(conversionResult);
        throw std::runtime_error(std::format("Failed to bind socket {}", WSAGetLastError()));
    }
    freeaddrinfo(conversionResult);
}

void SocketListener::listen() const {
    if (::listen(m_InternalSocket->socket, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error(std::format("Failed to listen on socket {}", WSAGetLastError()));
    }
}

void SocketListener::postAccept() const {
    sockaddr_storage clientAddress{};
    int clientAddressSize = sizeof(clientAddress);
    const auto clientSocket = accept(m_InternalSocket->socket, reinterpret_cast<sockaddr *>(&clientAddress),
                                     &clientAddressSize);
    if (clientSocket == INVALID_SOCKET) {
        throw std::runtime_error(std::format("Failed to accept socket {}", WSAGetLastError()));
    }
    closesocket(clientSocket);
}

void SocketListener::close() const {
    ::shutdown(m_InternalSocket->socket, SD_BOTH);
    cleanup();
}
