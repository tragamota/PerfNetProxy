#include <iostream>
#include <winsock2.h>

#include "socketListener.hpp"
#include "stub_.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    stub_();

    try {
        WSADATA startupData {};
        WSAStartup(MAKEWORD(2, 2), &startupData);

        SocketListener listener{InternetProtocolVersion::IPv4};
        listener.bind("0.0.0.0", 8080);
        listener.listen();

        while (true) {
            listener.postAccept();
        }

        listener.close();

        WSACleanup();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}