add_executable(ResourceServer
    ResourceServer.cpp
    CServer.cpp
    CSession.cpp
    LogicSystem.cpp
    LogicWorker.cpp
    FileWorker.cpp
    DownloadWorker.cpp
    FileSystem.cpp
    MsgNode.cpp
    MysqlDao.cpp
    MysqlMgr.cpp
    RedisMgr.cpp
    ConfigMgr.cpp
    AsioIOServicePool.cpp
    UserMgr.cpp
    base64.cpp
    ${CMAKE_SOURCE_DIR}/common/DistLock.cpp
)

target_include_directories(ResourceServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common      # Singleton.h, DistLock.h
)

target_link_libraries(ResourceServer PRIVATE
    Boost::boost
    Boost::system
    Boost::filesystem
    jsoncpp_lib
    hiredis::hiredis
    mysql-connector
)

target_compile_definitions(ResourceServer PRIVATE
    _WIN32_WINNT=0x0A00
)

if(WIN32)
    target_link_libraries(ResourceServer PRIVATE ws2_32)
endif()
