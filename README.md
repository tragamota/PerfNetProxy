# PerfNetProxy

**PerfNetProxy** is a high-performance, cross-platform asynchronous HTTP proxy written in modern C++.  
It leverages **OS-native network I/O** (epoll on Linux, IOCP on Windows) to deliver **low-latency, non-blocking request processing**. 
The project serves as a **systems-level showcase** of asynchronous networking, concurrency control, and performance-oriented C++ design.

---

## Features

- Cross-platform: Linux and Windows support
- Asynchronous, event-driven design
- OS-native network I/O (epoll / IOCP)
- Efficient request handling with minimal overhead
- Clear separation of transport, protocol parsing, and routing logic
- Lightweight and portable C++17+ codebase
- Personal project demonstrating advanced C++ and systems programming

---

## Architecture Overview

PerfNetProxy uses a **reactor/proactor-like architecture**:

1. **Listener / Acceptor** – Accepts incoming HTTP connections.
2. **Event Loop** – Uses OS-native async I/O for scalable, non-blocking operations.
   - Linux: **epoll**
   - Windows: **IOCP**
3. **Request Dispatcher** – Parses HTTP requests and forwards them to backend servers.
4. **Worker Threads** – Optional threading model for concurrency and scaling.
5. **Response Handling** – Sends responses asynchronously back to clients.

> Focused on **scalability, low latency, and minimal memory overhead**.

---

## Requirements

- C++17 or higher
- CMake 3.20+
- Compiler:
  - Linux: GCC 9+ or Clang 12+
  - Windows: MSVC 2019+
- Optional: Benchmarks require `ab` or `wrk` for load testing

---

## Building with CMake

```bash
# Clone repository
git clone https://github.com/yourusername/PerfNetProxy.git
cd PerfNetProxy

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build . --config Release 
``` 

PerfNetProxy/
├── src/              # Source files
│   ├── main.cpp
│   ├── server/       # Listener, Event Loop, OS-native I/O
│   ├── proxy/        # Request parsing and forwarding
│   └── util/         # Utilities, logging
├── include/          # Public headers
├── tests/            # Unit tests
├── CMakeLists.txt
└── README.md
