cmake_minimum_required(VERSION 3.10)

project(libdisqube)

file(GLOB_RECURSE LIB_SOURCES
    src/*.cpp
)

file(GLOB_RECURSE LIB_HEADERS
    src/*.hpp
)

add_library(disqube STATIC ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(disqube PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(REUSE_MODE=1)
    add_compile_definitions(DEBUG_MODE=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Wall -Wextra -pedantic -O3)
endif()

add_executable(Main src/disqube.cpp)
target_link_libraries(Main PRIVATE disqube)


install(TARGETS disqube
    EXPORT disqubeExport
    DESTINATION lib
)

install(FILES ${LIB_HEADERS}
    DESTINATION include
)

install(EXPORT disqubeExport
    FILE disqubeConfig.cmake
    NAMESPACE disqube::
    DESTINATION lib/cmake/disqube
)

if(EXISTS "${CMAKE_SOURCE_DIR}/test")
    add_subdirectory(test)

    enable_testing()
    add_test(NAME ByteBufferTest COMMAND bb_test)
    add_test(NAME MessageTest COMMAND message_test)
    add_test(NAME SocketTest COMMAND socket_test) 
    add_test(NAME ThreadTest COMMAND thread_test) 
    add_test(NAME QueueTest COMMAND queue_test)
    add_test(NAME CommunicationTest COMMAND comm_test)
    add_test(NAME InterfaceTest COMMAND interface_test)
    add_test(NAME LoggingTest COMMAND logging_test)
    add_test(NAME StateMachineTest COMMAND state_machine_test)
    add_test(NAME ProgressBarTest COMMAND progress_bar_test)
    add_test(NAME TimerTest COMMAND timer_test)
endif()