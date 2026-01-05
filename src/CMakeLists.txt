add_library(PerfNetProxyCore)

target_compile_features(PerfNetProxyCore PUBLIC cxx_std_20)

if(WIN32)
    target_sources(PerfNetProxyCore PRIVATE platform/windows/socketListener.cpp)

    target_include_directories(PerfNetProxyCore PUBLIC ${PROJECT_SOURCE_DIR}/include/platform)

    target_link_libraries(PerfNetProxyCore PRIVATE ws2_32)
elseif(APPLE)
    target_sources(PerfNetProxyCore PRIVATE )
elseif(UNIX)
#    target_sources(PerfNetProxyCore PRIVATE )
endif()

# Include headers
target_include_directories(PerfNetProxyCore PUBLIC ${PROJECT_SOURCE_DIR}/include)