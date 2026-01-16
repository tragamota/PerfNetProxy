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
            auto [initSource, bytesTransferred, taskContext] = completionQueue.GetCompletionTask();

            if (taskContext->operation == IOOperation::Accept) {
                const auto *acceptContext = reinterpret_cast<AcceptContext *>(taskContext);

                const auto source = std::get<SocketListener *>(initSource);
                const auto client = acceptContext->client;

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

                delete acceptContext;
            } else if (taskContext->operation == IOOperation::Send) {
                const auto client = std::get<SocketClient *>(initSource);
                const auto sendContext = std::unique_ptr<SendContext>(reinterpret_cast<SendContext *>(taskContext));


            }
            else if (taskContext->operation == IOOperation::Receive) {
                const auto client = std::get<SocketClient *>(initSource);
                const auto receiveContext = std::unique_ptr<ReceiveContext>(reinterpret_cast<ReceiveContext *>(taskContext));

                std::string str { reinterpret_cast<char *>(receiveContext->buffer.get()), bytesTransferred };

                std::cout << str << std::endl;

                auto bytes = std::as_bytes(std::span(sendExample));

                client->queueReceiveTask();
                client->queueSendTask({reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size()});
            }
        }

        completionQueue.close();
        listener.close();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
