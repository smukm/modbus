# Qt Modbus RTU Client

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-5.15%20|%206.x-green.svg)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A robust, feature-rich desktop application for communicating with Modbus RTU devices via serial (COM) ports. Built with C++ and the Qt Framework, it features a clean architecture, request queuing with priority, and comprehensive error handling.


## ✨ Features

- **Full Modbus RTU Support**: Read and Write operations using standard function codes (`0x01`, `0x02`, `0x03`, `0x04`, `0x05`, `0x06`, `0x0F`, `0x10`).
- **Smart Request Queuing**: Prevents bus collisions. Write requests are given **high priority** (prepended to queue), while read requests use normal priority (enqueued).
- **Periodic Polling**: Configurable auto-read timer for continuous data monitoring.
- **Advanced Error Handling**: Parses and displays human-readable Modbus Exception codes (e.g., *Illegal Data Address*, *Slave Device Busy*).
- **Real-time Data Grid**: Automatically updates and deduplicates register values in a clean table view.
- **Rotating Log System**: Keeps the UI responsive by limiting log entries (auto-removes oldest entries when exceeding the limit).
- **Strict Input Validation**: Prevents invalid addresses, function codes, and data formats from reaching the serial port.
- **Clean Architecture**: Separation of concerns using dedicated classes for UI state, data modeling, business logic, and validation.

## 🛠️ Prerequisites

- **Qt Framework**: Version 5.15 or Qt 6.x (requires `QtSerialPort` and `QtModbus` modules).
- **C++ Compiler**: C++17 or later recommended.
- **Build System**: CMake (recommended) or qmake.

## 🚀 Getting Started
```bash
### 1. Clone the repository
git clone https://github.com/smukm/modbus.git
cd modbus

### 2. Build the project
**Using Qt Creator (Recommended):**
1. Open Qt Creator.
2. Go to `File` -> `Open File or Project...` and select the `CMakeLists.txt` (or `.pro` file).
3. Configure the project with your preferred Desktop Qt Kit (e.g., MinGW 64-bit or MSVC).
4. Click the green **Run** button (or press `Ctrl+R`).

**Using Command Line (CMake):**
```bash
mkdir build && cd build
cmake ..
cmake --build .
```
