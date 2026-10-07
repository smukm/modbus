#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H
#include "modbusmanager.h"

struct ModbusLastParams {
    // Параметры чтения
    quint8 readDeviceAddr = 1;
    quint8 readFuncCode = 0x03;
    quint16 readStartAddr = 0;
    quint16 readCount = 1;

    // Параметры записи
    quint8 writeDeviceAddr = 1;
    quint8 writeFuncCode = 0x06;
    quint16 writeStartAddr = 0;
    quint16 writeCount = 1;
    QString writeData = "";
};

class SettingsManager
{
public:
    // Возвращает полный путь к файлу конфигурации
    static QString configFilePath();

    // Сохраняет настройки порта в JSON-файл. Возвращает true при успехе.
    static bool saveSettings(const ModbusConnectionSettings &settings);

    // Загружает настройки порта из JSON-файла.
    // Если файла нет или он поврежден, возвращает настройки по умолчанию.
    static ModbusConnectionSettings loadSettings();

    // Проверяет, существует ли файл конфигурации
    static bool settingsExist();

    // Последние параметры команд (чтение/запись)
    static bool saveLastParams(const ModbusLastParams &params);
    static ModbusLastParams loadLastParams();

private:
    // Вспомогательные методы для работы с корневым JSON-объектом
    static QJsonObject loadRootObject();
    static bool saveRootObject(const QJsonObject &rootObj);
};

#endif // SETTINGSMANAGER_H
