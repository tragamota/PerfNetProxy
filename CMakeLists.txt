cmake_minimum_required(VERSION 3.20)
project(PerfNetProxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

add_executable(PerfNetProxy src/main.cpp
        src/stub_.cpp)

target_link_libraries(PerfNetProxy PRIVATE PerfNetProxyCore)