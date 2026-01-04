add_library(PerfNetProxyCore)

target_compile_features(PerfNetProxyCore PUBLIC cxx_std_20)

if(WIN32)
    target_sources(PerfNetProxyCore PRIVATE platform/win32/socket_listener.cpp)

    target_link_libraries(PerfNetProxyCore PRIVATE ws2_32)
elseif(APPLE)
    target_sources(PerfNetProxyCore PRIVATE )
elseif(UNIX)
#    target_sources(PerfNetProxyCore PRIVATE )
endif()

# Include headers
target_include_directories(PerfNetProxyCore PUBLIC ${PROJECT_SOURCE_DIR}/include)