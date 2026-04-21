cmake_minimum_required(VERSION 3.16)

project(chat VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network)

# 1. 源文件列表 (全部加上 src/ 前缀)
set(CHAT_SOURCES
    src/adduseritem.cpp
    src/applyfriend.cpp
    src/applyfrienditem.cpp
    src/applyfriendlist.cpp
    src/applyfriendpage.cpp
    src/authenfriend.cpp
    src/bubbleframe.cpp
    src/chatdialog.cpp
    src/chatitembase.cpp
    src/chatpage.cpp
    src/chatuserlist.cpp
    src/chatuserwid.cpp
    src/chatview.cpp
    src/clickablelabel.cpp
    src/clickedbtn.cpp
    src/clickedlabel.cpp
    src/clickedoncelabel.cpp
    src/contactuserlist.cpp
    src/conuseritem.cpp
    src/customizeedit.cpp
    src/filetcpmgr.cpp
    src/findfaildlg.cpp
    src/findsuccessdlg.cpp
    src/friendinfopage.cpp
    src/friendlabel.cpp
    src/global.cpp
    src/grouptipitem.cpp
    src/httpmgr.cpp
    src/imagecropperlabel.cpp
    src/listitembase.cpp
    src/loadingdlg.cpp
    src/logindialog.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/messagetextedit.cpp
    src/picturebubble.cpp
    src/registerdialog.cpp
    src/resetdialog.cpp
    src/searchlist.cpp
    src/statewidget.cpp
    src/tcpmgr.cpp
    src/textbubble.cpp
    src/timerbtn.cpp
    src/userdata.cpp
    src/userinfopage.cpp
    src/usermgr.cpp
)

# 2. 头文件列表 (补全了 src/ 前缀)
set(CHAT_HEADERS
    src/adduseritem.h
    src/applyfriend.h
    src/applyfrienditem.h
    src/applyfriendlist.h
    src/applyfriendpage.h
    src/authenfriend.h
    src/bubbleframe.h
    src/chatdialog.h
    src/chatitembase.h
    src/chatpage.h
    src/chatuserlist.h
    src/chatuserwid.h
    src/chatview.h
    src/clickablelabel.h
    src/clickedbtn.h
    src/clickedlabel.h
    src/clickedoncelabel.h
    src/contactuserlist.h
    src/conuseritem.h
    src/customizeedit.h
    src/filetcpmgr.h
    src/findfaildlg.h
    src/findsuccessdlg.h
    src/friendinfopage.h
    src/friendlabel.h
    src/global.h
    src/grouptipitem.h
    src/httpmgr.h
    src/imagecropperdialog.h
    src/imagecropperlabel.h
    src/listitembase.h
    src/loadingdlg.h
    src/logindialog.h
    src/mainwindow.h
    src/messagetextedit.h
    src/picturebubble.h
    src/registerdialog.h
    src/resetdialog.h
    src/searchlist.h
    src/singleton.h
    src/statewidget.h
    src/tcpmgr.h
    src/textbubble.h
    src/timerbtn.h
    src/userdata.h
    src/userinfopage.h
    src/usermgr.h
)

# 3. 界面文件列表 (全部加上 src/ 前缀)
set(CHAT_FORMS
    src/adduseritem.ui
    src/applyfriend.ui
    src/applyfrienditem.ui
    src/applyfriendpage.ui
    src/authenfriend.ui
    src/chatdialog.ui
    src/chatpage.ui
    src/chatuserwid.ui
    src/conuseritem.ui
    src/findfaildlg.ui
    src/findsuccessdlg.ui
    src/friendinfopage.ui
    src/friendlabel.ui
    src/grouptipitem.ui
    src/loadingdlg.ui
    src/logindialog.ui
    src/mainwindow.ui
    src/registerdialog.ui
    src/resetdialog.ui
    src/userinfopage.ui
)

# 4. 资源文件 (假设 rc.qrc 在根目录，如果也在 src 下请改为 src/rc.qrc)
set(CHAT_RESOURCES
    rc.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(chat
        MANUAL_FINALIZATION
        ${CHAT_SOURCES}
        ${CHAT_HEADERS}
        ${CHAT_FORMS}
        ${CHAT_RESOURCES}
    )
else()
    add_executable(chat
        ${CHAT_SOURCES}
        ${CHAT_HEADERS}
        ${CHAT_FORMS}
        ${CHAT_RESOURCES}
    )
endif()

target_include_directories(chat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(chat PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

if(MSVC)
    target_compile_options(chat PRIVATE /wd4819 /utf-8)
endif()

target_link_libraries(chat PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

add_custom_command(TARGET chat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
            "$<TARGET_FILE_DIR:chat>/config.ini"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/static"
            "$<TARGET_FILE_DIR:chat>/static"
)

if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(chat)
endif()