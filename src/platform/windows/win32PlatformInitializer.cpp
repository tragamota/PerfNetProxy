//
// Created by Ian on 12-1-2026.
//

#include <format>
#include <stdexcept>
#include <winsock2.h>

#include "platformInitializer.hpp"

PlatformInitializer::PlatformInitializer() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error(std::format("WSAStartup failed - {}", WSAGetLastError()));
    };
}

PlatformInitializer::~PlatformInitializer() {
    WSACleanup();
}
