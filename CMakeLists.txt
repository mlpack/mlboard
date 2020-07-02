cmake_minimum_required(VERSION 3.3.2)
project(mlboard 
        LANGUAGES C CXX)

# Set required C++ standard to C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests." ON)
INCLUDE(FindProtobuf)
FIND_PACKAGE(Protobuf REQUIRED)

ADD_SUBDIRECTORY(include/proto)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB headerfiles "${CMAKE_CURRENT_SOURCE_DIR}/include/filewriter/*.hpp")

file(COPY ${headerfiles}
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/mlboard/filewriter/)

file(GLOB headerfiles "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

file(COPY ${headerfiles}
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/mlboard/)

add_library(mlboard INTERFACE)
include_directories(${CMAKE_INCLUDE_PATH})
target_include_directories(mlboard INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/mlboard)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/proto
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/proto/libproto.a
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)

# Enable testing and build tests.
enable_testing()
if (BUILD_TESTS)
  add_subdirectory(tests)
endif()