cmake_minimum_required(VERSION 3.10)
project(json_reflection_server)

set(CMAKE_CXX_STANDARD 17)

# Find all required packages
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(nlohmann_json REQUIRED)

# Include Protobuf headers
include_directories(${Protobuf_INCLUDE_DIRS})

# Define proto file and output paths
get_filename_component(PROTO_FILE "./proto/json_reflection.proto" ABSOLUTE)
get_filename_component(PROTO_PATH "${PROTO_FILE}" DIRECTORY)

set(PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/json_reflection.pb.cc")
set(PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/json_reflection.pb.h")
set(PROTO_GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/json_reflection.grpc.pb.cc")
set(PROTO_GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/json_reflection.grpc.pb.h")

add_custom_command(
    OUTPUT "${PROTO_SRCS}" "${PROTO_HDRS}" "${PROTO_GRPC_SRCS}" "${PROTO_GRPC_HDRS}"
    COMMAND protoc
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         -I "${PROTO_PATH}"
         --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`
         "${PROTO_FILE}"
    DEPENDS "${PROTO_FILE}"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


# Add the server executable
add_executable(server
    json_reflection_server.cpp
    ${PROTO_SRCS}
    ${PROTO_GRPC_SRCS}
)

# Link against necessary libraries
target_link_libraries(server
    PRIVATE
    gRPC::grpc++
    gRPC::grpc++_reflection    # ✅ Add this
    protobuf::libprotobuf
    nlohmann_json::nlohmann_json
)
