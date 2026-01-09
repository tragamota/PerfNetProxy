#include <iostream>
#include <winsock2.h>

#include "completionQueue.hpp"
#include "socketListener.hpp"

int main() {
    try {
        WSADATA startupData {};
        WSAStartup(MAKEWORD(2, 2), &startupData);

        CompletionQueue completionQueue;
        SocketListener listener{InternetProtocolVersion::IPv4};

        listener.bind("0.0.0.0", 8080);
        listener.listen();

        completionQueue.AddSocketListenerReference(listener);

        listener.acceptClient();
        completionQueue.GetCompletionTask();

        listener.close();

        WSACleanup();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}