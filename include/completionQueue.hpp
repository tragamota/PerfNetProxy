//
// Created by Ian on 7-1-2026.
//

#ifndef PERFNETPROXY_COMPLETIONQUEUE_H
#define PERFNETPROXY_COMPLETIONQUEUE_H

#include <memory>

#include "platform/windows/ioContext.hpp"
#include "socketListener.hpp"

struct InternalCompletionQueue;

class CompletionQueue {
     std::unique_ptr<InternalCompletionQueue> m_InternalCompletionQueue;
public:
     CompletionQueue();
     ~CompletionQueue();

     CompletionQueue(const CompletionQueue&) = delete;
     CompletionQueue& operator=(const CompletionQueue&) = delete;

     CompletionQueue(CompletionQueue&& other) noexcept = default;
     CompletionQueue& operator=(CompletionQueue&& other) noexcept = default;

     void AddSocketListenerReference(const SocketListener&) const;

     [[nodiscard]] IOContext* GetCompletionTask() const;
};


#endif //PERFNETPROXY_COMPLETIONQUEUE_H