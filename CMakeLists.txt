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
