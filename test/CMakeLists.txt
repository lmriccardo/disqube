cmake_minimum_required(VERSION 3.10)

# Adding all test executable
add_executable(bb_test ../test/bb.cpp)
add_executable(message_test ../test/message.cpp)
add_executable(socket_test ../test/socket.cpp)

# Linking executables to the distributed libraries
target_link_libraries(bb_test PRIVATE distributed)
target_link_libraries(message_test PRIVATE distributed)
target_link_libraries(socket_test PRIVATE distributed)