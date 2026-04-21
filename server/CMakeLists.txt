cmake_minimum_required(VERSION 3.20)
project(Badchat LANGUAGES CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# MySQL Connector/C++ (not available via Conan, use local installation)
set(MYSQL_CONNECTOR_ROOT "" CACHE PATH "Path to MySQL Connector/C++ installation (e.g. D:/cppsoft/mysql_connector)")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(common)

add_subdirectory(proto)
add_subdirectory(common)
add_subdirectory(services/gate)
add_subdirectory(services/chat)
add_subdirectory(services/status)
add_subdirectory(services/resource)
