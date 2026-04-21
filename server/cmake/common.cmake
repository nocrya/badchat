# Find all required packages (provided by Conan or system)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(gRPC REQUIRED CONFIG)
find_package(protobuf REQUIRED CONFIG)
find_package(hiredis REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(OpenSSL REQUIRED)

# MySQL Connector/C++ manual discovery
if(MYSQL_CONNECTOR_ROOT)
    set(_mysql_root "${MYSQL_CONNECTOR_ROOT}")
else()
    set(_mysql_root "D:/cppsoft/mysql_connector")
endif()

find_path(MYSQL_CONNECTOR_INCLUDE_DIR
    NAMES jdbc/mysql_driver.h
    PATHS "${_mysql_root}/include"
    NO_DEFAULT_PATH
)

find_library(MYSQL_CONNECTOR_LIB
    NAMES mysqlcppconn
    PATHS "${_mysql_root}/lib64/vs14/debug" "${_mysql_root}/lib64/vs14"
    NO_DEFAULT_PATH
)

if(NOT MYSQL_CONNECTOR_INCLUDE_DIR OR NOT MYSQL_CONNECTOR_LIB)
    message(FATAL_ERROR
        "MySQL Connector/C++ not found.\n"
        "Set -DMYSQL_CONNECTOR_ROOT=<path> or install to ${_mysql_root}")
endif()

add_library(mysql-connector IMPORTED INTERFACE)
set_target_properties(mysql-connector PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${MYSQL_CONNECTOR_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${MYSQL_CONNECTOR_LIB}"
)

message(STATUS "MySQL Connector include: ${MYSQL_CONNECTOR_INCLUDE_DIR}")
message(STATUS "MySQL Connector lib: ${MYSQL_CONNECTOR_LIB}")

# Convenience function to add a server target
function(add_badchat_server TARGET_NAME)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES;EXTRA_LIBS")

    add_executable(${TARGET_NAME} ${ARG_SOURCES})

    target_link_libraries(${TARGET_NAME} PRIVATE
        badchat-common
        badchat-proto
        mysql-connector
        gRPC::grpc++
        protobuf::libprotobuf
        OpenSSL::SSL
        OpenSSL::Crypto
        ${ARG_EXTRA_LIBS}
    )

    if(WIN32)
        target_link_libraries(${TARGET_NAME} PRIVATE ws2_32)
    endif()
endfunction()
