#include <format>
#include <iostream>
#include <span>
#include <sstream>
#include <winsock2.h>

#include "completionQueue.hpp"
#include "platformInitializer.hpp"
#include "socketListener.hpp"

int main() {
    PlatformInitializer platformInitializer {};

    try {
        CompletionQueue completionQueue;
        SocketListener listener{InternetProtocolVersion::IPv4};

        listener.bind("0.0.0.0", 8080);
        listener.listen();

        completionQueue.AddSocketListenerReference(listener);

        auto sendExample = std::string{
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 11\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello world"
        };

        for (int i = 0; i < 100; i++) {
            listener.acceptClient();
        }

        while (true) {
            auto task = completionQueue.GetCompletionTask();

            if (task.taskContext->operation == IOOperation::Accept) {
                const auto *acceptContext = reinterpret_cast<AcceptContext *>(task.taskContext);

                const auto source = std::get<SocketListener *>(task.source);
                const auto client = acceptContext->ClientSocket;

                completionQueue.AddSocketClientReference(*client);

                if (setsockopt(
                    client->getInternalSocket()->socket,
                    SOL_SOCKET,
                    SO_UPDATE_ACCEPT_CONTEXT,
                    reinterpret_cast<const char *>(&source->getInternalSocket()->socket),
                    sizeof(source->getInternalSocket()->socket)) == SOCKET_ERROR)
                {
                    std::cout << "Failed to set accept context" << WSAGetLastError() << std::endl;
                }

                int optval = 1;
                if (setsockopt(client->getInternalSocket()->socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&optval), sizeof(int)) == SOCKET_ERROR) {
                    std::cout << "Failed to set TCP_NODELAY" << WSAGetLastError() << std::endl;
                }

                client->queueReceiveTask();
                listener.acceptClient();

                delete acceptContext->outputBuffer;
                delete acceptContext;
            } else if (task.taskContext->operation == IOOperation::Send) {
                const auto client = std::get<SocketClient *>(task.source);
                const auto *sendContext = reinterpret_cast<SendContext *>(task.taskContext);

                delete sendContext->buffer;
                delete sendContext;
            }
            else if (task.taskContext->operation == IOOperation::Receive) {
                const auto client = std::get<SocketClient *>(task.source);
                const auto *receiveContext = reinterpret_cast<ReceiveContext *>(task.taskContext);

                std::string str { reinterpret_cast<char *>(receiveContext->buffer), task.bytesTransferred };

                std::cout << str << std::endl;

                auto bytes = std::as_bytes(std::span(sendExample));

                client->queueReceiveTask();
                client->queueSendTask({reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size()});

                delete receiveContext->buffer;
                delete receiveContext;
            }
        }

        completionQueue.close();
        listener.close();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
