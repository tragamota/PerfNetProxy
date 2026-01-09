//
// Created by Ian on 7-1-2026.
//

#include <format>
#include <iostream>
#include <ostream>

#include "completionQueue.hpp"
#include "socketClient.hpp"
#include "../../../include/platform/windows/ioContext.hpp"




struct InternalCompletionQueue {
    HANDLE completionPortHandle;
};

CompletionQueue::CompletionQueue() : m_InternalCompletionQueue(std::make_unique<InternalCompletionQueue>()) {
    m_InternalCompletionQueue->completionPortHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,
        nullptr,
        0,
        0);
}

CompletionQueue::~CompletionQueue() {
    ::CloseHandle(m_InternalCompletionQueue->completionPortHandle);
}

void CompletionQueue::AddSocketListenerReference(const SocketListener &listener) const {
    ::CreateIoCompletionPort(
        reinterpret_cast<HANDLE>(listener.getInternalSocket()->socket),
        m_InternalCompletionQueue->completionPortHandle,
        0,
        0);
}

void CompletionQueue::AddSocketClientReference(const SocketClient& client) const {
    ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(client->)),
    m_InternalCompletionQueue->completionPortHandle,
    0,
    0);
}

IOContext* CompletionQueue::GetCompletionTask() const {
    uint32_t bytesReturned = 0;
    ULONG_PTR completionKey = 0;
    LPOVERLAPPED overlapped;

    if (::GetQueuedCompletionStatus(m_InternalCompletionQueue->completionPortHandle, reinterpret_cast<LPDWORD>(&bytesReturned), &completionKey, &overlapped, INFINITE) == FALSE) {
        std::cout << "GetQueuedCompletionStatus failed " << WSAGetLastError() << std::endl;
    }

    return reinterpret_cast<IOContext *>(overlapped);
}
