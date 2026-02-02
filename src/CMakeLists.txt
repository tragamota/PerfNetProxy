add_library(PerfNetProxyCore)

target_compile_features(PerfNetProxyCore PUBLIC cxx_std_20)

target_sources(PerfNetProxyCore PRIVATE proxyApplication.cpp httpParser.cpp)

if(WIN32)
    target_sources(PerfNetProxyCore PRIVATE platform/windows/win32SocketListener.cpp platform/windows/win32CompletionQueue.cpp platform/windows/win32SocketClient.cpp platform/windows/win32PlatformInitializer.cpp)

    target_include_directories(PerfNetProxyCore PUBLIC ${PROJECT_SOURCE_DIR}/include/platform/windows)

    target_link_libraries(PerfNetProxyCore PRIVATE ws2_32 mswsock)
elseif(APPLE)
    target_sources(PerfNetProxyCore PRIVATE )
elseif(UNIX)
    target_sources(PerfNetProxyCore PRIVATE )
endif()

# Include headers
target_include_directories(PerfNetProxyCore PUBLIC ${PROJECT_SOURCE_DIR}/include)