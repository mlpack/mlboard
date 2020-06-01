cmake_minimum_required(VERSION 3.3.2)
project(mlboard 
        LANGUAGES C CXX)

# Set required C++ standard to C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


ADD_SUBDIRECTORY(include/proto)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_library(mlboard INTERFACE)
target_include_directories(mlboard INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/mlboard)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/proto
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)