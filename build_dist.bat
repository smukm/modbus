@echo off
chcp 65001 >nul
setlocal

echo ============================================
echo   Сборка дистрибутива ModbusClient
echo ============================================
echo.

REM ===== НАСТРОЙКИ =====
set QT_DIR=C:/Qt/6.11.2/mingw_64
set MINGW_DIR=C:/Qt/Tools/mingw1310_64
set CMAKE_DIR=C:/Qt/Tools/CMake_64
set BUILD_TYPE=Release
set INSTALL_DIR=dist
set PROJECT_NAME=ModbusClient
REM =================================================

REM Проверка путей
if not exist "%QT_DIR%" (
    echo [ОШИБКА] Qt не найден: %QT_DIR%
    pause
    exit /b 1
)
if not exist "%MINGW_DIR%\bin\gcc.exe" (
    echo [ОШИБКА] MinGW gcc не найден: %MINGW_DIR%\bin\gcc.exe
    pause
    exit /b 1
)
if not exist "%CMAKE_DIR%\bin\cmake.exe" (
    echo [ОШИБКА] CMake не найден: %CMAKE_DIR%\bin\cmake.exe
    echo.
    echo Попробуйте найти cmake.exe вручную:
    echo   dir /s /b C:\Qt\Tools\cmake.exe
    echo   dir /s /b "C:\Program Files\CMake\bin\cmake.exe"
    pause
    exit /b 1
)

REM Добавляем CMake и MinGW в PATH
set PATH=%CMAKE_DIR%\bin;%MINGW_DIR%\bin;%PATH%

echo [ИНФО] Используем:
echo   CMake:  %CMAKE_DIR%\bin\cmake.exe
echo   MinGW:  %MINGW_DIR%\bin\gcc.exe
echo   Qt:     %QT_DIR%
echo.

REM Удаляем старую сборку
echo [1/5] Очистка старых файлов...
if exist build rmdir /s /q build
if exist %INSTALL_DIR% rmdir /s /q %INSTALL_DIR%
echo      Готово.
echo.

REM Конфигурация
echo [2/5] Конфигурация CMake...
cmake -B build -G "MinGW Makefiles" ^
      -DCMAKE_PREFIX_PATH="%QT_DIR%" ^
      -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
      -DCMAKE_C_COMPILER="%MINGW_DIR%/bin/gcc.exe" ^
      -DCMAKE_CXX_COMPILER="%MINGW_DIR%/bin/g++.exe"
if errorlevel 1 (
    echo [ОШИБКА] Конфигурация не удалась!
    pause
    exit /b 1
)
echo      Готово.
echo.

REM Сборка
echo [3/5] Сборка проекта...
cmake --build build
if errorlevel 1 (
    echo [ОШИБКА] Сборка не удалась!
    pause
    exit /b 1
)
echo      Готово.
echo.

REM Установка в dist
echo [4/5] Установка в %INSTALL_DIR% (windeployqt + MinGW runtime)...
cmake --install build --prefix %INSTALL_DIR%
if errorlevel 1 (
    echo [ОШИБКА] Установка не удалась!
    pause
    exit /b 1
)
echo      Готово.
echo.

REM Создание установщика
echo [5/5] Создание установщика NSIS...
cpack --config build/CPackConfig.cmake -B %INSTALL_DIR% -G NSIS
if errorlevel 1 (
    echo [ОШИБКА] Создание установщика не удалось!
    pause
    exit /b 1
)
echo      Готово.
echo.

REM ZIP-архив
echo [БОНУС] Создание портативного ZIP...
cpack --config build/CPackConfig.cmake -B %INSTALL_DIR% -G ZIP
echo      Готово.
echo.

echo ============================================
echo   СБОРКА ЗАВЕРШЕНА УСПЕШНО!
echo ============================================
echo.
echo   Дистрибутив находится в папке: %INSTALL_DIR%
echo.

explorer %INSTALL_DIR%

pause
endlocal