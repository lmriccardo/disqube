cmake_minimum_required(VERSION 3.10)

project(libdistributed)

file(GLOB_RECURSE LIB_SOURCES
    distributed/*.cpp
)

file(GLOB_RECURSE LIB_HEADERS
    distributed/*.hpp
)

add_library(distributed STATIC ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(distributed PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/distributed>
    $<INSTALL_INTERFACE:include>
)

add_executable(Main distributed/main.cpp)
target_link_libraries(Main PRIVATE distributed)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(REUSE_MODE=1)
    add_compile_definitions(DEBUG_MODE=1)
endif()

install(TARGETS distributed
    EXPORT distributedExport
    DESTINATION lib
)

install(FILES ${LIB_HEADERS}
    DESTINATION include
)

install(EXPORT distributedExport
    FILE distributedConfig.cmake
    NAMESPACE distributed::
    DESTINATION lib/cmake/distributed
)

add_subdirectory(test)

enable_testing()
add_test(NAME ByteBufferTest COMMAND bb_test)
add_test(NAME MessageTest COMMAND message_test)
add_test(NAME SocketTest COMMAND socket_test) 
add_test(NAME ThreadTest COMMAND thread_test) 
add_test(NAME QueueTest COMMAND queue_test)