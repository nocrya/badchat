set(PROTO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/message.proto")
set(PROTO_OUT "${CMAKE_CURRENT_BINARY_DIR}")

# Force Conan's protoc (must match library version, NOT system protoc)
unset(PROTOC_EXECUTABLE CACHE)
find_program(PROTOC_EXECUTABLE protoc
    PATHS "${protobuf_PACKAGE_FOLDER_RELEASE}/bin"
          "${protobuf_PACKAGE_FOLDER_DEBUG}/bin"
    NO_DEFAULT_PATH
    REQUIRED
)

# Force Conan's grpc_cpp_plugin
unset(GRPC_CPP_PLUGIN CACHE)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin
    PATHS "${grpc_PACKAGE_FOLDER_RELEASE}/bin"
          "${grpc_PACKAGE_FOLDER_DEBUG}/bin"
    NO_DEFAULT_PATH
    REQUIRED
)

message(STATUS "protoc: ${PROTOC_EXECUTABLE}")
message(STATUS "grpc_cpp_plugin: ${GRPC_CPP_PLUGIN}")

set(PROTO_GENERATED_SRCS
    "${PROTO_OUT}/message.pb.cc"
    "${PROTO_OUT}/message.pb.h"
    "${PROTO_OUT}/message.grpc.pb.cc"
    "${PROTO_OUT}/message.grpc.pb.h"
)

add_custom_command(
    OUTPUT ${PROTO_GENERATED_SRCS}
    COMMAND ${PROTOC_EXECUTABLE}
        --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
        --cpp_out=${PROTO_OUT}
        --grpc_out=${PROTO_OUT}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        ${PROTO_SRC}
    DEPENDS ${PROTO_SRC}
    COMMENT "Generating protobuf/gRPC sources from message.proto"
)

add_library(badchat-proto STATIC
    ${PROTO_GENERATED_SRCS}
)

target_include_directories(badchat-proto PUBLIC
    ${PROTO_OUT}
)

target_link_libraries(badchat-proto PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)
