cmake_minimum_required(VERSION 3.10)

# Adding all test executable
add_executable(bb_test ../test/bb.cpp)
add_executable(message_test ../test/message.cpp)
add_executable(socket_test ../test/socket.cpp)
add_executable(thread_test ../test/thread.cpp)
add_executable(queue_test ../test/queue.cpp)
add_executable(comm_test ../test/comm.cpp)
add_executable(interface_test ../test/interface.cpp)
add_executable(config_test ../test/config.cpp)

# Linking executables to the distributed libraries
target_link_libraries(bb_test PRIVATE disqube)
target_link_libraries(message_test PRIVATE disqube)
target_link_libraries(socket_test PRIVATE disqube)
target_link_libraries(thread_test PRIVATE disqube)
target_link_libraries(queue_test PRIVATE disqube)
target_link_libraries(comm_test PRIVATE disqube)
target_link_libraries(interface_test PRIVATE disqube)
target_link_libraries(config_test PRIVATE disqube)