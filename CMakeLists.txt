cmake_minimum_required(VERSION 3.16)

project(serialport VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialPort SerialBus LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort SerialBus LinguistTools)


set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    modbusmanager.h modbusmanager.cpp
    logmanager.h logmanager.cpp
    registerdatamodel.h registerdatamodel.cpp
    modbusvalidator.h modbusvalidator.cpp
    uicontroller.h uicontroller.cpp
    settingsmanager.h settingsmanager.cpp
    resources.qrc
)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/en_US.ts
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(serialport
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(serialport SHARED ${PROJECT_SOURCES})
    else()
        add_executable(serialport ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(serialport PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::SerialBus
)

# 2. Генерация переводов ПОСЛЕ создания executable.
# RESOURCE_PREFIX автоматически добавит .qm файлы в ресурсы по пути /translations
qt_add_translations(serialport
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/translations"
)

# Стандартные свойства проекта
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.serialport)
endif()

set_target_properties(serialport PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS serialport
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(serialport)
endif()