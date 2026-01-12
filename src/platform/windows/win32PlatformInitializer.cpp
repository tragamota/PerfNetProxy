//
// Created by Ian on 12-1-2026.
//

#include <stdexcept>
#include <winsock2.h>

#include "platformInitializer.hpp"

PlatformInitializer::PlatformInitializer() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    };
}

PlatformInitializer::~PlatformInitializer() {
    WSACleanup();
}
