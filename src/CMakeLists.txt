add_library(PerfNetProxyCore)

target_sources(PerfNetProxyCore PRIVATE stub_.cpp)

if(WIN32)
#    target_sources(PerfNetProxyCore PRIVATE )
elseif(APPLE)
#    target_sources(PerfNetProxyCore PRIVATE )
elseif(UNIX)
#    target_sources(PerfNetProxyCore PRIVATE )
endif()

# Include headers
target_include_directories(PerfNetProxyCore PUBLIC ${PROJECT_SOURCE_DIR}/include)