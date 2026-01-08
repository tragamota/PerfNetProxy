cmake_minimum_required(VERSION 3.20)
project(PerfNetProxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_MODULE_STD ON)

if (MSVC)
    add_compile_options(/experimental:module)
endif()

add_subdirectory(src)

add_executable(PerfNetProxy src/main.cpp
)

target_link_libraries(PerfNetProxy PRIVATE PerfNetProxyCore)