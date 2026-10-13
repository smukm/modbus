cmake_minimum_required(VERSION 3.16)

project(modbusclient VERSION 0.1 LANGUAGES CXX)

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

    logmanager.h logmanager.cpp
    registerdatamodel.h registerdatamodel.cpp
    modbusvalidator.h modbusvalidator.cpp
    uicontroller.h uicontroller.cpp
    settingsmanager.h settingsmanager.cpp
    resources.qrc
)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/ru_RU.ts  # Русский перевод
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(modbusclient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        abstractmodbusmanager.h abstractmodbusmanager.cpp
        modbusrtumanager.h modbusrtumanager.cpp
        modbustcpmanager.h modbustcpmanager.cpp
        preferencesdlg.h preferencesdlg.cpp preferencesdlg.ui
    )
else()
    if(ANDROID)
        add_library(modbusclient SHARED ${PROJECT_SOURCES})
    else()
        add_executable(modbusclient ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(modbusclient PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::SerialBus
)

# 2. Генерация переводов ПОСЛЕ создания executable.
# RESOURCE_PREFIX автоматически добавит .qm файлы в ресурсы по пути /translations
qt_add_translations(modbusclient
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/translations"
    TARGETS modbusclient
)

# Стандартные свойства проекта
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.modbusclient)
endif()

set_target_properties(modbusclient PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS modbusclient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(modbusclient)
endif()

# ============== DEPLOYMENT ==============
# Генерирует скрипт, который при выполнении cmake --install сам запустит аналог windeployqt.
if(QT_VERSION VERSION_GREATER_EQUAL 6.3)
    qt_generate_deploy_app_script(
        TARGET modbusclient
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

# ============== Добавление библиотек MinGW ==============
if(MINGW)
    get_filename_component(MINGW_BIN ${CMAKE_CXX_COMPILER} DIRECTORY)

    # Копируем runtime библиотеки напрямую через install(FILES)
    foreach(LIB libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
        if(EXISTS "${MINGW_BIN}/${LIB}")
            install(FILES "${MINGW_BIN}/${LIB}" DESTINATION bin)
        endif()
    endforeach()
endif()

# ============== Упаковка в дистрибутив CPACK ==============
set(CPACK_PACKAGE_NAME "ModbusClient")
set(CPACK_PACKAGE_VENDOR "SmukM")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modbus Serial Port Tool")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ModbusClient")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "ModbusClient ${PROJECT_VERSION}")
    set(CPACK_NSIS_PACKAGE_NAME "ModbusClient")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
endif()

include(CPack)