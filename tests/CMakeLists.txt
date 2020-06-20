# The tests that need to be compiled.
set(MLBOARD_TESTS_SOURCES
    main.cpp
    mock_test.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(mlboard_tests ${MLBOARD_TESTS_SOURCES})

target_include_directories(mlboard_tests PRIVATE ${CMAKE_BINARY_DIR}/include ${PROTOBUF_INCLUDE_DIR})

target_link_libraries(mlboard_tests 
    mlboard 
    ${CMAKE_BINARY_DIR}/include/proto/libproto.a
    ${PROTOBUF_LIBRARIES})


enable_testing()
add_test(NAME mlboard_tests COMMAND mlboard_tests
WORKING_DIRECTORY ${CMAKE_BINARY_DIR})