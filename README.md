# Qt Modbus Client (RTU & TCP)

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-5.15%20|%206.x-green.svg)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A robust, feature-rich desktop application for communicating with Modbus devices via **Serial (RTU)** and **Ethernet (TCP)**. Built with C++17 and the Qt Framework, it features a clean architecture, intelligent request queuing, persistent settings, and comprehensive error handling.

## ✨ Features

- **Dual Protocol Support**: Seamlessly switch between Modbus RTU (Serial COM ports) and Modbus TCP (IP/Port).
- **Full Modbus Function Codes**: Read and Write operations using standard codes (`0x01`, `0x02`, `0x03`, `0x04`, `0x05`, `0x06`, `0x0F`, `0x10`).
- **Smart Request Queuing**: Prevents bus collisions. Write requests are given **high priority** (prepended to the queue), while read requests use normal priority (enqueued).
- **Intelligent Device Exclusion**: Features a sliding-window timeout tracker. Automatically excludes unresponsive devices from polling after a configurable number of timeouts to prevent bus lockups, with a one-click manual reset.
- **Periodic Polling**: Configurable auto-read timer for continuous, real-time data monitoring.
- **Advanced Error Handling**: Parses and displays human-readable Modbus Exception codes (e.g., *Illegal Data Address*, *Slave Device Busy*) and critical connection errors.
- **Persistent Settings**: Automatically saves and restores connection parameters and the last used read/write command parameters via JSON (`SettingsManager`).
- **Real-time Data Grid**: Automatically updates, formats, and deduplicates register values in a clean, scrollable table view (`RegisterDataModel`).
- **Strict Input Validation**: Prevents invalid addresses, function codes, and data formats from reaching the port via a dedicated `ModbusValidator`.
- **Clean Architecture**: Strict separation of concerns using dedicated classes for UI state (`UiController`), data modeling, business logic (`AbstractModbusManager`), and logging (`LogManager`).

## 🛠️ Prerequisites

- **Qt Framework**: Version Qt 6.x.
- **Required Qt Modules**: `Core`, `Gui`, `Widgets`, `SerialPort`, `Modbus`.
- **C++ Compiler**: C++17 or later recommended.
- **Build System**: CMake (recommended) or qmake.

## 🚀 Getting Started

### 1. Clone the repository
```bash
git clone https://github.com/smukm/modbus.git
cd modbus