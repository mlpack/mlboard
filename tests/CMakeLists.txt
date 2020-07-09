# The tests that need to be compiled.
set(MLBOARD_TESTS_SOURCES
    main.cpp
    filewriter_test.cpp
    summarywriter_test.cpp
    util_test.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(mlboard_tests ${MLBOARD_TESTS_SOURCES})

set(MLBOARD_INCLUDE_DIRS ${MLPACK_INCLUDE_DIR}
                        ${ARMADILLO_INCLUDE_DIR}
                        ${PROTOBUF_INCLUDE_DIR}
                        ${Boost_INCLUDE_DIRS})

set(MLBOARD_LIBRARIES ${MLPACK_LIBRARY}
                     ${ARMADILLO_LIBRARIES}
                     ${PROTOBUF_LIBRARIES}
                     ${Boost_LIBRARIES})

target_include_directories(mlboard_tests PRIVATE ${CMAKE_BINARY_DIR}/include ${MLBOARD_INCLUDE_DIRS})

target_link_libraries(mlboard_tests 
    mlboard 
    proto
    ${MLBOARD_LIBRARIES})


enable_testing()
add_test(NAME mlboard_tests COMMAND mlboard_tests
WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
