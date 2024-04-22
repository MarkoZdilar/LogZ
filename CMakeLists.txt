cmake_minimum_required(VERSION 3.5)

project(LogZ VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH "/Users/markozdilar/Qt/6.6.2/macos")
list(APPEND CMAKE_PREFIX_PATH "/Users/markozdilar/local")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(KF6Archive REQUIRED)

set(TS_FILES LogZ_en_HR.ts)

set(PROJECT_SOURCES
        main.cpp
        View/src/mainwindow.cpp
        View/inc/mainwindow.h
        View/src/mainwindow.ui
        View/inc/fileitemdelegate.h
        View/src/fileitemdelegate.cpp
        View/inc/clickabletextedit.h
        View/src/clickabletextedit.cpp
        View/inc/customtextedit.h
        View/src/customtextedit.cpp
        View/inc/finddialog.h
        View/src/finddialog.cpp
        View/inc/zipviewerdialog.h
        View/src/zipviewerdialog.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LogZ
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(LogZ SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(LogZ
            ${PROJECT_SOURCES}
        )
    endif()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_include_directories(LogZ PRIVATE View/inc)

target_link_libraries(LogZ PRIVATE Qt${QT_VERSION_MAJOR}::Widgets KF6::Archive)

set_target_properties(LogZ PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LogZ
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LogZ)
endif()
