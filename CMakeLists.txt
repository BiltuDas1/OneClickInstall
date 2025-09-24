cmake_minimum_required(VERSION 3.16)

project(OneClickInstall VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(CURL REQUIRED)

# Root
set(SRC_MAIN
    windows/main.cpp
)

# Groups
set(SRC_UI
    windows/ui/mainwindow.h
    windows/ui/mainwindow_includes.h
    windows/ui/mainwindow.cpp
    windows/ui/mainwindow.ui
    windows/ui/buttons/download.cpp
    windows/ui/buttons/browse.cpp
)

set(SRC_CORE
    windows/core/install.h
    windows/core/install.cpp
    windows/core/download.h
    windows/core/download.cpp
)

set(SRC_UTILS
    windows/utils/network/network.h
    windows/utils/network/network.cpp
)

set(PROJECT_SOURCES
    ${SRC_MAIN}
    ${SRC_UI}
    ${SRC_CORE}
    ${SRC_UTILS}
)

if(WIN32)
    set(RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/OneClickInstall.rc")
    list(APPEND PROJECT_SOURCES ${RESOURCE_FILE})
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OneClickInstall
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET OneClickInstall APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(OneClickInstall SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(OneClickInstall
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(OneClickInstall PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    CURL::libcurl
    psl
    idn2
    ssh2
    nghttp2
    ssl
    crypto
    zstd
    z
    brotlidec
    brotlicommon
    unistring
    iconv
    ws2_32
    wldap32
    crypt32
    secur32
    bcrypt
)

# Static Linking Curl
if(DEFINED ENV{LINK_STATICALLY})
    target_compile_definitions(OneClickInstall PRIVATE CURL_STATICLIB)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.OneClickInstall)
endif()
set_target_properties(OneClickInstall PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS OneClickInstall
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OneClickInstall)
endif()
