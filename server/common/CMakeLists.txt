add_library(badchat-common STATIC
    ConfigMgr.cpp
    AsioIOServicePool.cpp
    DistLock.cpp
)

target_include_directories(badchat-common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(badchat-common PUBLIC
    Boost::boost
    Boost::system
    Boost::filesystem
    jsoncpp_lib
    hiredis::hiredis
)

target_compile_definitions(badchat-common PUBLIC
    _WIN32_WINNT=0x0A00
)
