# ============================================================================
# common.cmake —— badchat 各 server 共用的 CMake 配置
#
# 主要职责：
#   1. find_package 查找 Conan 安装的依赖（Boost/gRPC/Protobuf/hiredis/…）
#   2. 定位 MySQL Connector/C++（Conan 目前没提供，手动查找）
#   3. 提供 add_badchat_server() 辅助函数，把“新建 exe + 链接公共库 + 拷贝运行时 DLL”一次性封装好
# ============================================================================

# ----------------------------------------------------------------------------
# 1) Conan 依赖
# ----------------------------------------------------------------------------
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(gRPC REQUIRED CONFIG)
find_package(protobuf REQUIRED CONFIG)
find_package(hiredis REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(OpenSSL REQUIRED)

# ----------------------------------------------------------------------------
# 2) MySQL Connector/C++（头文件 + import lib）
#    - 默认去 D:/cppsoft/mysql_connector 找，可用 -DMYSQL_CONNECTOR_ROOT=... 覆盖
#    - Release 的 .lib 在 lib64/vs14/，Debug 的在 lib64/vs14/debug/
# ----------------------------------------------------------------------------
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

# ----------------------------------------------------------------------------
# 3) 运行时 DLL 目录（随仓库一起提交，省得每次手动拷）
#
#    项目内存放必要 DLL：server/third_party/mysql_connector/*.dll
#    build 时会自动把里面的 .dll 全部拷到 exe 旁边。
#
#    如果以后还有别的第三方 DLL 需要跟随发布（比如 OpenSSL 的 libcrypto-3-x64.dll
#    等），直接丢进这个目录就行，不必改 CMake。
# ----------------------------------------------------------------------------
set(BADCHAT_RUNTIME_DLL_DIR "${CMAKE_SOURCE_DIR}/third_party/mysql_connector"
    CACHE PATH "Directory containing runtime DLLs to be copied next to every server exe")

file(GLOB BADCHAT_RUNTIME_DLLS "${BADCHAT_RUNTIME_DLL_DIR}/*.dll")
message(STATUS "Runtime DLLs to copy: ${BADCHAT_RUNTIME_DLLS}")

# ----------------------------------------------------------------------------
# 4) add_badchat_server() —— 新增一个 server exe 的快捷方式
#
#    用法示例（见 services/chat/CMakeLists.txt）：
#       add_badchat_server(ChatServer
#           SOURCES
#               ChatServer.cpp
#               CSession.cpp
#               ...
#           EXTRA_LIBS
#               某个额外库     # 可选
#       )
#
#    它做了 3 件事：
#      a. 创建 exe
#      b. 链接公共库（common / proto / mysql / gRPC / OpenSSL / Boost 等）
#      c. Windows 下构建结束后自动把 BADCHAT_RUNTIME_DLLS 里的 .dll 拷到 exe 目录
# ----------------------------------------------------------------------------
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

        # 运行时 DLL 拷贝：第一次构建会拷，之后只有 DLL 更新才会重新拷
        if(BADCHAT_RUNTIME_DLLS)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${BADCHAT_RUNTIME_DLLS}
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND_EXPAND_LISTS
                VERBATIM
                COMMENT "Copying runtime DLLs to $<TARGET_FILE_DIR:${TARGET_NAME}>"
            )
        endif()
    endif()
endfunction()
