include_directories(${Protobuf_INCLUDE_DIRS})
file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")

protobuf_generate_cpp(ProtoSources ProtoHeaders ${ProtoFiles})
ADD_LIBRARY(proto ${ProtoHeaders} ${ProtoSources})
