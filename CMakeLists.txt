cmake_minimum_required(VERSION 3.10)

# Create the project
project(DisqubeProject)

# Find all source and header files
file(GLOB_RECURSE LIB_SOURCES src/*.cpp )
file(GLOB_RECURSE LIB_HEADERS src/*.hpp )

# Create the library called disqube
add_library(disqube STATIC ${LIB_SOURCES} ${LIB_HEADERS})
target_include_directories(disqube PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Add compile definitions and compile options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(REUSE_MODE=1)
    add_compile_definitions(DEBUG_MODE=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Wall -Wextra -pedantic -O3)
endif()

# Add the main executable and link libraries
add_executable(disqubed src/disqube.cpp)
target_link_libraries(disqubed PRIVATE disqube)

# Store the binary into the bin folder
set_target_properties(disqubed PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Add tests
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
    add_test(NAME ArgparseTest COMMAND argparse_test)
endif()