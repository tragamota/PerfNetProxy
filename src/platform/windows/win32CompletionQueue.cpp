//
// Created by Ian on 7-1-2026.
//

#include <format>
#include <iostream>
#include <ostream>

#include "completionQueue.hpp"
#include "completionTask.hpp"
#include "socketClient.hpp"
#include "ioContext.hpp"


struct InternalCompletionQueue {
    HANDLE completionPortHandle = INVALID_HANDLE_VALUE;
};

CompletionQueue::CompletionQueue() : m_InternalCompletionQueue(std::make_unique<InternalCompletionQueue>()) {
    m_InternalCompletionQueue->completionPortHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                                                               nullptr,
                                                                               0,
                                                                               0);
}

CompletionQueue::~CompletionQueue() {
    if (m_InternalCompletionQueue->completionPortHandle != INVALID_HANDLE_VALUE) {
        close();
    }
}

void CompletionQueue::AddSocketListenerReference(const SocketListener &listener) const {
    ::CreateIoCompletionPort(
        reinterpret_cast<HANDLE>(listener.getInternalSocket()->socket),
        m_InternalCompletionQueue->completionPortHandle,
        reinterpret_cast<ULONG_PTR>(&listener),
        0);
}

void CompletionQueue::AddSocketClientReference(const SocketClient &client) const {
    ::CreateIoCompletionPort(
        reinterpret_cast<HANDLE>(client.getInternalSocket()->socket),
        m_InternalCompletionQueue->completionPortHandle,
        reinterpret_cast<ULONG_PTR>(&client),
        0);
}

CompletionTask CompletionQueue::GetCompletionTask() const {
    size_t bytesReturned = 0;
    ULONG_PTR completionKey = 0;
    LPOVERLAPPED overlapped;

    CompletionTask task {};

    if (::GetQueuedCompletionStatus(m_InternalCompletionQueue->completionPortHandle,
                                    reinterpret_cast<LPDWORD>(&bytesReturned), &completionKey, &overlapped,
                                    INFINITE) == FALSE) {
        std::cout << "GetQueuedCompletionStatus failed " << WSAGetLastError() << std::endl;
    }

    task.taskContext = reinterpret_cast<IOBaseContext *>(overlapped);
    task.bytesTransferred = bytesReturned;

    if (task.taskContext->operation == IOOperation::Accept) {
        task.initSource = reinterpret_cast<SocketListener *>(completionKey);
    }
    else {
        task.initSource = reinterpret_cast<SocketClient *>(completionKey);
    }

    return task;
}

void CompletionQueue::close() const {
    ::CloseHandle(m_InternalCompletionQueue->completionPortHandle);
    m_InternalCompletionQueue->completionPortHandle = INVALID_HANDLE_VALUE;
}

